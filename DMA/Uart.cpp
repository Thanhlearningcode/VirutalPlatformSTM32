#include "Uart.h"
#include <memory>
#include <chrono>
#include "../Common/BusInterface/PeripheralRegistry.h"


Uart::Uart(const char* name) : Master(name), Slave(name), baudrate(9600), busy(false), error(false), data_reg(0)
{
    this->Name = name;
    this->periph_base = 0x40000000; // default base
    // register read callback: at offset 0, pop up to len bytes from TX FIFO
    // Expose a larger window so DMA can request multiple bytes at once
    printf("UART: Registering periph at 0x%08x size=0x%X\n", this->periph_base, 0x1000);
    PeripheralRegistry::Register(this->periph_base, 0x1000, [this](uint32_t offset, uint8_t* out, size_t len)->bool {
        if (offset == 0 && len > 0) {
            printf("UART: read callback request len=%zu\n", len);
            size_t copied = 0;
            // wait until at least one byte available, then drain up to len
            std::unique_lock<std::mutex> lk(this->fifoMutex);
            this->fifoCv.wait(lk, [this](){ return !this->txFifo.empty() || !this->alive.load(); });
            while (copied < len && !this->txFifo.empty()) {
                out[copied++] = this->txFifo.front();
                this->txFifo.pop_front();
            }
            printf("UART: provided %zu bytes\n", copied);
            // Keep data_reg mirrored to last byte for compatibility
            if (copied > 0) this->data_reg = out[copied - 1];
            return copied > 0;
        }
        return false;
    });
}

// Ensure threads waiting on UART FIFO can exit if any
Uart::~Uart()
{
    alive.store(false);
    fifoCv.notify_all();
}

void Uart::SetBaudrate(unsigned int br) {
    baudrate = br;
}

bool Uart::IsBusy() const {
    return busy;
}

bool Uart::HasError() const {
    return error;
}

void Uart::Reset() {
    busy = false;
    error = false;
    data_reg = 0;
    // clear FIFO
    {
        std::lock_guard<std::mutex> g(fifoMutex);
        txFifo.clear();
    }
}

void Uart::SetData(unsigned char d) {
    data_reg = d;
    // also push to FIFO so DMA can consume it
    PushTx(d);
}

unsigned char Uart::GetData() const {
    return data_reg;
}

bool Uart::Transmit(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng truyền dữ liệu UART
    if (package && package->Length > 0) {
        data_reg = package->Buffer[0];
        PushTx(package->Buffer, package->Length);
    }
    busy = false;
    return true;
}

bool Uart::Received(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng nhận dữ liệu UART
    if (package && package->Length > 0) {
        package->Buffer[0] = data_reg;
    }
    busy = false;
    return true;
}

void Uart::PushTx(const uint8_t* data, size_t len)
{
    if (!data || len == 0) return;
    {
        std::lock_guard<std::mutex> g(fifoMutex);
        for (size_t i = 0; i < len; ++i) txFifo.push_back(data[i]);
    printf("UART: PushTx len=%zu first=%02X\n", len, len?data[0]:0);
    }
    fifoCv.notify_one();
}

void Uart::PushTx(uint8_t b)
{
    {
        std::lock_guard<std::mutex> g(fifoMutex);
        txFifo.push_back(b);
    }
    printf("UART: PushTx byte=%02X\n", b);
    fifoCv.notify_one();
}

size_t Uart::PopTx(uint8_t* out, size_t len, uint32_t timeout_ms)
{
    if (!out || len == 0) return 0;
    std::unique_lock<std::mutex> lk(fifoMutex);
    if (timeout_ms == 0) {
        fifoCv.wait(lk, [this](){ return !txFifo.empty() || !alive.load(); });
    } else {
        fifoCv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [this](){ return !txFifo.empty() || !alive.load(); });
    }
    size_t copied = 0;
    while (copied < len && !txFifo.empty()) {
        out[copied++] = txFifo.front();
        txFifo.pop_front();
    }
    return copied;
}
