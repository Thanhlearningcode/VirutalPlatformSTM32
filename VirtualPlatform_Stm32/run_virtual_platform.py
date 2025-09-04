#!/usr/bin/env python3

import http.server
import socketserver
import threading
import subprocess
import os
import json
import time

PORT = 8081
WEB_DIR = os.path.join(os.path.dirname(__file__), '.')

# Simple in-memory CAN state for two nodes
can_state = {
    1: { 'baud': 500000, 'last_rx': None },
    2: { 'baud': 500000, 'last_rx': None }
}

# Simple in-memory UART state (for demo)
uart_state = {
    'data': [],  # list of bytes
    'baud': 9600
}

# Simulated SRAM (64KB)
SRAM_SIZE = 64 * 1024
sram = bytearray(SRAM_SIZE)

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    # Keep track of SSE clients
    sse_clients = []

    def send_sse(self, event, data):
        payload = f"event: {event}\n"
        payload += f"data: {json.dumps(data)}\n\n"
        dead = []
        for w in list(self.sse_clients):
            try:
                w.write(payload.encode())
                w.flush()
            except Exception:
                dead.append(w)
        for d in dead:
            try:
                self.sse_clients.remove(d)
            except Exception:
                pass

    def do_POST(self):
        length = int(self.headers.get('Content-Length', 0))
        data = self.rfile.read(length)
        try:
            req = json.loads(data)
        except:
            req = {}
        resp = {}
        if self.path == '/api/reset':
            resp = {"status": "reset", "pc": "0x00000000", "sp": "0x00000000", "lr": "0x00000000"}
        elif self.path == '/api/step':
            resp = {"status": "stepped", "pc": "0x00000004", "sp": "0x20000000", "lr": "0x00000000"}
        elif self.path == '/api/uart':
            # store UART data into uart_state for demo transfers
            d = req.get('data')
            # accept string or list of bytes
            if isinstance(d, str):
                # store ASCII bytes
                uart_state['data'] = [ord(c) for c in d]
            elif isinstance(d, list):
                uart_state['data'] = [int(x) & 0xFF for x in d]
            else:
                uart_state['data'] = [int(d) & 0xFF] if d is not None else []
            uart_state['baud'] = int(req.get('baud', uart_state['baud']))
            resp = {"dr": uart_state['data'], "sr": "0x01"}
        elif self.path == '/api/spi':
            resp = {"dr": req.get("data", "0x00"), "sr": "0x01"}
        elif self.path == '/api/i2c':
            resp = {"dr": req.get("data", "0x00"), "sr": "0x01"}
        elif self.path == '/api/timer':
            resp = {"cnt": req.get("value", 0), "sr": "0x01"}
        elif self.path == '/api/dma':
            # return current DMA register-like info if requested
            resp = {"ccr": "0x00", "cndtr": 0, "cpar": 0, "cmar": 0}
            # if request contains transfer command, handle it
            # expected: { action: 'transfer', cpar: <int>, cmar: <int>, cndtr: <int> }
            if req.get('action') == 'transfer':
                cpar = int(req.get('cpar', 0))
                cmar = int(req.get('cmar', 0))
                cndtr = int(req.get('cndtr', 0))
                # For demo: if cpar matches UART peripheral base (0x40000000), read from uart_state
                # otherwise if cpar in range of sram, copy from sram region
                try:
                    if cpar == 0x40000000:
                        data_bytes = uart_state.get('data', [])[:cndtr]
                    else:
                        # read from sram if valid
                        if 0 <= cpar < SRAM_SIZE and cpar + cndtr <= SRAM_SIZE:
                            data_bytes = list(sram[cpar:cpar+cndtr])
                        else:
                            data_bytes = [0] * cndtr
                    # write into sram at cmar
                    if 0 <= cmar < SRAM_SIZE and cmar + len(data_bytes) <= SRAM_SIZE:
                        for i, b in enumerate(data_bytes):
                            sram[cmar + i] = int(b) & 0xFF
                        resp = { 'status': 'ok', 'written': len(data_bytes), 'cmar': cmar }
                    else:
                        resp = { 'status': 'error', 'reason': 'cmar out of range' }
                except Exception as e:
                    resp = { 'status': 'error', 'reason': str(e) }
        # CAN endpoints
        elif self.path == '/api/can/send':
            # req expected: { src:1, id: <int>, dlc: <int>, extended: <bool>, data: [bytes...] }
            src = int(req.get('src', 1))
            dst = 2 if src == 1 else 1
            frame = {
                'id': int(req.get('id', 0)),
                'dlc': int(req.get('dlc', 0)),
                'extended': bool(req.get('extended', False)),
                'data': list(req.get('data', []))
            }
            # store in destination's last_rx
            can_state[dst]['last_rx'] = frame
            resp = {'status': 'sent', 'src': src, 'dst': dst}
            # broadcast SSE to connected UI clients about this frame
            try:
                self.send_sse('can', {'from': src, 'to': dst, 'frame': frame})
            except Exception:
                pass
        elif self.path == '/api/can/recv':
            # req expected: { node: 1 }
            node = int(req.get('node', 1))
            frame = can_state.get(node, {}).get('last_rx')
            if frame is None:
                resp = {'empty': True}
            else:
                # return and clear
                resp = {'empty': False, 'frame': frame}
                can_state[node]['last_rx'] = None
        elif self.path == '/api/can/set':
            # req expected: { node:1, baud:500000 }
            node = int(req.get('node', 1))
            baud = int(req.get('baud', can_state.get(node, {}).get('baud', 500000)))
            if node in can_state:
                can_state[node]['baud'] = baud
                resp = {'status': 'ok', 'node': node, 'baud': baud}
            else:
                resp = {'error': 'invalid node'}

        # send JSON response for POST
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(resp).encode())

    def do_GET(self):
        # Provide an SSE endpoint at /api/events
        if self.path == '/api/events':
            self.send_response(200)
            self.send_header('Content-Type', 'text/event-stream')
            self.send_header('Cache-Control', 'no-cache')
            self.send_header('Connection', 'keep-alive')
            self.end_headers()
            # register client wfile for pushing events
            w = self.wfile
            self.sse_clients.append(w)
            try:
                # keep connection open
                while True:
                    # heartbeat
                    try:
                        w.write(b": ping\n\n")
                        w.flush()
                    except Exception:
                        break
                    time.sleep(15)
            finally:
                try:
                    self.sse_clients.remove(w)
                except Exception:
                    pass
            return
        elif self.path.startswith('/api/sram/read'):
            # query param style: /api/sram/read?addr=128&len=16
            from urllib.parse import urlparse, parse_qs
            q = urlparse(self.path).query
            params = parse_qs(q)
            addr = int(params.get('addr', [0])[0])
            length = int(params.get('len', [16])[0])
            if 0 <= addr < SRAM_SIZE and addr + length <= SRAM_SIZE:
                chunk = list(sram[addr:addr+length])
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'addr': addr, 'len': length, 'data': chunk}).encode())
                return
            else:
                self.send_response(400)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'error': 'out of range'}).encode())
                return
        else:
            return super().do_GET()

# Start backend C++ process (dummy for now)
def start_backend():
    print("Backend started (dummy)")

if __name__ == "__main__":
    threading.Thread(target=start_backend, daemon=True).start()
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        print(f"Serving UI at http://localhost:{PORT}")
        httpd.serve_forever()
