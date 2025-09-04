#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DMA_DIR="$ROOT_DIR/DMA"
UI_DIR="$ROOT_DIR/VirtualPlatform_Stm32"

usage() {
  cat <<EOF
Usage: $(basename "$0") [command]

Commands:
  ui           Run the Web UI demo (Python server on http://localhost:8081)
  build        Build C++ modules for DMA/UART demo
  test         Build and run the C++ UART→DMA threaded demo
  clean        Clean C++ build outputs
  all          Build, run test, then start the UI
  help         Show this help

Defaults to: ui
EOF
}

need() {
  command -v "$1" >/dev/null 2>&1 || { echo "Missing dependency: $1" >&2; exit 1; }
}

build_cpp() {
  need make
  need g++
  echo "[build] Building C++ modules in DMA/ ..."
  (cd "$DMA_DIR" && make -s clean && make -s test >/dev/null)
  echo "[build] Done. Binary: $DMA_DIR/test_dma"
}

run_test() {
  build_cpp
  echo "[test] Running C++ UART→DMA demo ..."
  set +e
  (cd "$DMA_DIR" && ./test_dma)
  local rc=$?
  set -e
  if [[ $rc -ne 0 ]]; then
    echo "[test] Demo exited with code $rc" >&2
    exit $rc
  fi
}

run_ui() {
  need python3
  echo "[ui] Starting Web UI (Python) ..."
  echo "[ui] URL: http://localhost:8081"
  echo "[ui] Press Ctrl+C to stop."
  cd "$UI_DIR"
  exec python3 run_virtual_platform.py
}

clean() {
  echo "[clean] Cleaning C++ objects in DMA/ ..."
  (cd "$DMA_DIR" && make -s clean)
  echo "[clean] Done."
}

cmd="${1:-ui}"
case "$cmd" in
  ui) run_ui ;;
  build) build_cpp ;;
  test) run_test ;;
  clean) clean ;;
  all)
    build_cpp
    run_test
    run_ui
    ;;
  help|-h|--help) usage ;;
  *) echo "Unknown command: $cmd" >&2; usage; exit 1 ;;
 esac
