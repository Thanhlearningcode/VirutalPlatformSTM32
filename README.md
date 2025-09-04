# 🧠 Virtual STM32 Platform

Dự án Virtual STM32 là một hệ thống mô phỏng phần mềm vi điều khiển STM32, hỗ trợ học tập và phát triển hệ thống nhúng theo mô hình chuẩn. Dự án xây dựng môi trường mô phỏng bằng C++ (thuần), có tích hợp kiểm thử và một UI demo đơn giản.

---

## 🏗️ Kiến trúc tổng quan

- ⚙️ Ngôn ngữ: C++14/17
- 🧪 Kiểm thử: Python (minh hoạ) + test C++ nhỏ
- 🔧 Build: Makefile
- 📜 Tự động hóa: Bash + Python script
- 💡 Mục tiêu: Mô phỏng register, bus, GPIO, DMA, UART và lõi CPU (Cortex‑M)

---

## 📁 Cấu trúc thư mục

| Thư mục | Nội dung |
|--------|---------|
| `Common/` | Định nghĩa dùng chung (bus, registry, port, register model) |
| `CorTex-M3/` | Mô phỏng lõi CPU ARM Cortex‑M3 |
| `DMA/` | Mô phỏng DMA, UART, SPI, I2C, CAN, SRAM, kèm test |
| `ENV_ARM/` | Cấu hình môi trường mô phỏng |
| `VirtualPlatform_Stm32/` | UI web demo + backend Python |
| `README.md` | Tài liệu này |

---

## 🚀 Tính năng nổi bật

- Mô hình hoá bộ nhớ, thanh ghi và bus tương tự STM32
- Modular, dễ mở rộng (Port, CPU, RAM, DMA, UART, ...)
- DMA nền thread và UART FIFO thread‑safe (condition_variable + unique_lock)
- Makefile chuẩn, dễ build và thử nghiệm nhanh

---

## ⚙️ Yêu cầu môi trường

- GCC/G++ (C++14 trở lên), Make
- Python 3.8+ (để chạy UI demo)

---

## 🧵 UART → DMA (threaded, condition_variable + unique_lock)

Đã triển khai truyền dữ liệu từ UART sang SRAM thông qua DMA theo kiểu bất đồng bộ:

- UART có TX FIFO thread‑safe (mutex + condition_variable)
- DMA có worker thread xử lý copy sang SRAM
- Ghi thanh ghi DMA (CCR/CPAR/CMAR/CNDTR) sẽ kích hoạt transfer
- DMA đọc dữ liệu UART qua PeripheralRegistry tại địa chỉ 0x40000000

Build & chạy demo C++:

```bash
cd DMA
make clean
make test
./test_dma
```

Kỳ vọng: In ra dump SRAM có chuỗi “Hello from UART to DMA” và PASS.

Tệp chính:
- `DMA/Uart.h`, `DMA/Uart.cpp`: UART + FIFO TX + condition_variable/unique_lock
- `DMA/DMA.h`, `DMA/DMA.cpp`: Worker thread, callback thanh ghi, queue transfer
- `DMA/DMA_Resigter.*`: Map thanh ghi DMA + callback ghi
- `DMA/SRAM.*`: SRAM thread‑safe
- `Common/BusInterface/PeripheralRegistry.*`: Đăng ký ngoại vi và đọc theo địa chỉ
- `DMA/Makefile`, `DMA/test_dma.cpp`: Build + demo

---

## 🖥️ Chạy Web UI (demo)

UI mô phỏng ngoại vi cơ bản (UART/SPI/I2C/Timer/DMA/CAN) bằng Python HTTP server:

```bash
cd VirtualPlatform_Stm32
python3 run_virtual_platform.py
```

Mở trình duyệt: http://localhost:8081

Lưu ý: Backend Python mang tính minh hoạ; luồng UART→DMA gốc hiện thực bằng C++ ở thư mục `DMA/`.

---

## 🧪 Ví dụ test (Python, minh hoạ)

```python
# test_gpio.py
def test_port_signal_read():
    port = Port()
    signal = Signal(1)
    port(signal)
    assert port.Read_Data() == 1
```

---

## 📎 Ghi chú

- Nếu cần tích hợp C++ backend vào UI Python (thay vì demo), có thể mở API đọc/ghi thanh ghi để gọi vào lớp DMA/UART.
- Có thể tắt log debug trong `DMA/*.cpp` khi ổn định.
