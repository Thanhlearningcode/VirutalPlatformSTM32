// Minimal modular JS to wire up the UI to the backend endpoints (matching run_virtual_platform.py)

const $ = (q) => document.querySelector(q);

function log(el, msg) {
  const p = document.createElement('div');
  p.textContent = `[${new Date().toLocaleTimeString()}] ${msg}`;
  el.appendChild(p);
  el.scrollTop = el.scrollHeight;
}

async function http(method, url, body) {
  const opt = { method, headers: { 'Content-Type': 'application/json' } };
  if (body && method !== 'GET') opt.body = JSON.stringify(body);
  const res = await fetch(url, opt);
  if (!res.ok) throw new Error(`${method} ${url} -> ${res.status}`);
  return res.json().catch(() => ({}));
}

function setupCPU() {
  const logEl = $('#cpu-log');
  const resetBtn = $('#btn-cpu-reset');
  const stepBtn = $('#btn-cpu-step');
  if (resetBtn) resetBtn.addEventListener('click', async () => {
    try { const data = await http('POST', '/api/reset', {}); log(logEl, `Reset -> ${JSON.stringify(data)}`); } catch(e) { log(logEl, e.message); }
  });
  if (stepBtn) stepBtn.addEventListener('click', async () => {
    try { const data = await http('POST', '/api/step', {}); log(logEl, `Step -> ${JSON.stringify(data)}`); } catch(e) { log(logEl, e.message); }
  });
}

function setupUART() {
  const logEl = $('#uart-log');
  const sendBtn = $('#btn-uart-send');
  if (!sendBtn) return;
  sendBtn.addEventListener('click', async () => {
    const text = $('#uart-text').value || '';
    const baud = parseInt($('#uart-baud').value || '9600');
    try {
      const data = await http('POST', '/api/uart', { data: text, baud });
      log(logEl, `TX -> ${text}`);
      if (Array.isArray(data?.dr)) {
        const rx = String.fromCharCode(...data.dr);
        log(logEl, `Stored on server (RX emu): ${rx}`);
      }
    } catch (e) { log(logEl, e.message); }
  });
}

function setupSPI() {
  const logEl = $('#spi-log');
  const xferBtn = $('#btn-spi-xfer');
  if (!xferBtn) return;
  xferBtn.addEventListener('click', async () => {
    const hex = $('#spi-hex').value || '';
    try {
      const data = await http('POST', '/api/spi', { data: hex });
      log(logEl, `XFER ${hex} -> ${JSON.stringify(data)}`);
    } catch (e) { log(logEl, e.message); }
  });
}

function setupI2C() {
  const logEl = $('#i2c-log');
  const xferBtn = $('#btn-i2c-xfer');
  if (!xferBtn) return;
  xferBtn.addEventListener('click', async () => {
    const addr = parseInt($('#i2c-addr').value || '0');
    const hex = $('#i2c-hex').value || '';
    try {
      const data = await http('POST', '/api/i2c', { data: hex, addr });
      log(logEl, `XFER @0x${addr.toString(16)} ${hex} -> ${JSON.stringify(data)}`);
    } catch (e) { log(logEl, e.message); }
  });
}

function setupTimer() {
  const logEl = $('#timer-log');
  const setBtn = $('#btn-timer-set');
  const startBtn = $('#btn-timer-start');
  const stopBtn = $('#btn-timer-stop');
  if (setBtn) setBtn.addEventListener('click', async () => {
    const value = parseInt($('#timer-value').value || '0');
    try { const data = await http('POST', '/api/timer', { value }); log(logEl, `Set -> ${JSON.stringify(data)}`); } catch(e) { log(logEl, e.message); }
  });
  if (startBtn) startBtn.addEventListener('click', () => log(logEl, 'Timer started (UI only)'));
  if (stopBtn) stopBtn.addEventListener('click', () => log(logEl, 'Timer stopped (UI only)'));
}

function setupDMA() {
  const logEl = $('#dma-log');
  const runBtn = $('#btn-dma-xfer');
  const readBtn = $('#btn-sram-read');
  if (runBtn) runBtn.addEventListener('click', async () => {
    const len = parseInt($('#dma-len').value || '16');
    const cparStr = $('#dma-cpar').value || '0x40000000';
    const cmar = parseInt($('#dma-cmar').value || '128');
    // parse cpar possibly hex
    const cpar = cparStr.trim().startsWith('0x') ? parseInt(cparStr, 16) : parseInt(cparStr, 10);
    try {
      const data = await http('POST', '/api/dma', { action: 'transfer', cpar, cmar, cndtr: len });
      log(logEl, `DMA start len=${len}, cpar=${cparStr}, cmar=${cmar} -> ${JSON.stringify(data)}`);
    } catch (e) { log(logEl, e.message); }
  });
  if (readBtn) readBtn.addEventListener('click', async () => {
    // reuse DMA CMAR and Length fields for readback
    const addr = parseInt($('#dma-cmar').value || '128');
    const len = parseInt($('#dma-len').value || '32');
    try {
      const data = await http('GET', `/api/sram/read?addr=${addr}&len=${len}`);
      log(logEl, `SRAM [${addr}..${addr+len}) -> ${JSON.stringify(data)}`);
    } catch (e) { log(logEl, e.message); }
  });
}

function setupCAN() {
  const logEl = $('#can-log');
  // SSE events
  try {
    const evt = new EventSource('/api/events');
    evt.addEventListener('can', (m) => log(logEl, `EVT ${m.data}`));
  } catch {}
  const sendBtn = $('#btn-can-send');
  if (!sendBtn) return;
  sendBtn.addEventListener('click', async () => {
    const src = parseInt($('#can-src').value || '1');
    const id = parseInt($('#can-id').value || '0');
    const dlc = parseInt($('#can-dlc').value || '0');
    const extended = $('#can-ext').checked;
    const raw = ($('#can-data').value || '').split(',').map(s => s.trim()).filter(Boolean).map(s => parseInt(s, 16) & 0xFF);
    try {
      const data = await http('POST', '/api/can/send', { src, id, dlc, extended, data: raw });
      log(logEl, `CAN TX src=${src} id=0x${id.toString(16)} data=${JSON.stringify(raw)} -> ${JSON.stringify(data)}`);
    } catch (e) { log(logEl, e.message); }
  });
}

window.addEventListener('DOMContentLoaded', () => {
  setupCPU();
  setupUART();
  setupSPI();
  setupI2C();
  setupTimer();
  setupDMA();
  setupCAN();
});
