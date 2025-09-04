#!/usr/bin/env python3

import http.server
import socketserver
import threading
import subprocess
import os
import json
import time

PORT = 8080
WEB_DIR = os.path.join(os.path.dirname(__file__), '.')

# Simple in-memory CAN state for two nodes
can_state = {
    1: { 'baud': 500000, 'last_rx': None },
    2: { 'baud': 500000, 'last_rx': None }
}

# Backend subprocess (C++ binary)
backend_proc = None
backend_lock = threading.Lock()

def start_cpp_backend():
    global backend_proc
    with backend_lock:
        if backend_proc is None or backend_proc.poll() is not None:
            bin_path = os.path.join(os.path.dirname(__file__), 'backend', 'vp_backend')
            if not os.path.exists(bin_path):
                # try to build
                try:
                    subprocess.check_call(['make', '-C', os.path.join(os.path.dirname(__file__), 'backend')])
                except Exception:
                    print('Failed to build backend')
            try:
                backend_proc = subprocess.Popen([bin_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1)
                print('Started C++ backend', bin_path)
            except Exception as e:
                print('Error starting backend:', e)

def backend_request_line(line, timeout=1.0):
    # send a line and read one response line
    start_cpp_backend()
    if backend_proc is None:
        return None
    with backend_lock:
        try:
            backend_proc.stdin.write(line + '\n')
            backend_proc.stdin.flush()
            # read a single response line
            resp = backend_proc.stdout.readline()
            return resp.strip() if resp is not None else None
        except Exception as e:
            print('backend_request_line error', e)
            return None

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
            resp = {"dr": req.get("data", "0x00"), "sr": "0x01"}
        elif self.path == '/api/spi':
            resp = {"dr": req.get("data", "0x00"), "sr": "0x01"}
        elif self.path == '/api/i2c':
            resp = {"dr": req.get("data", "0x00"), "sr": "0x01"}
        elif self.path == '/api/timer':
            resp = {"cnt": req.get("value", 0), "sr": "0x01"}
        elif self.path == '/api/dma':
            resp = {"ccr": "0x01", "cndtr": "0x10", "cpar": "0x20", "cmar": "0x30"}
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
