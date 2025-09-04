# 🧠 Virtual STM32 Platform

Dự án **Virtual STM32** là một hệ thống mô phỏng phần mềm vi điều khiển STM32, được thiết kế để hỗ trợ học tập và phát triển hệ thống nhúng theo mô hình chuẩn hóa. Dự án tập trung vào việc xây dựng **môi trường mô phỏng vi điều khiển** bằng C++ thuần, tích hợp kiểm thử với Python, tự động build bằng Bash script và Makefile để mô phỏng hoạt động tương tự như một firmware thực.

---

## 🏗️ Kiến trúc tổng quan

- ⚙️ **Ngôn ngữ chính:** `C++17`  
- 🧪 **Kiểm thử:** `Python` (pytest hoặc script test đơn giản)
- 🔧 **Build system:** `Makefile`  
- 📜 **Tự động hóa:** `Bash script` (`build.sh`, `run_test.sh`, v.v.)
- 💡 **Mục tiêu:** Mô phỏng register, bus, GPIO, và cấu trúc lõi CPU (ARM Cortex-M)

---

## 📁 Cấu trúc thư mục

| Thư mục | Nội dung |
|--------|---------|
| `Common/` | Các định nghĩa dùng chung (enum, macro, signal class...) |
| `CorTex-M3/` | Mô phỏng lõi CPU ARM Cortex-M3 |
| `SRAM/` | Mô phỏng bộ nhớ SRAM nội bộ |
| `DMA/` | Mô phỏng bộ điều khiển truy cập trực tiếp DMA |
| `ENV_ARM/` | Tập tin cấu hình môi trường mô phỏng ARM |
| `README.md` | Giới thiệu tổng thể dự án |

---

## 🚀 Tính năng nổi bật

- ✨ Mô hình hóa bộ nhớ, thanh ghi, và bus giao tiếp tương tự STM32
- 🔁 Giao tiếp tín hiệu giữa các module với lớp `Signal<Type>`
- 🧵 Thiết kế theo hướng **modular** dễ mở rộng (Port, CPU, RAM, DMA, ...)
- 🧪 Tích hợp test Python: xác minh hành vi mô phỏng thông qua script tự động
- ⚡ Bash script giúp build & test nhanh gọn
- 🛠️ Sử dụng Makefile chuẩn hóa để build như firmware thật

---

## 🧪 Ví dụ test (Python)

```python
# test_gpio.py
def test_port_signal_read():
    port = Port()
    signal = Signal(1)
    port(signal)
    assert port.Read_Data() == 1
