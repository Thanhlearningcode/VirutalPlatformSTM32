#pragma once

#include "BusInterface.h"
#include "Port.h"

#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>


class Uart : public Master, public Slave
{
private:
    const char* Name;
    unsigned int baudrate;
    bool busy;
    bool error;
    unsigned char data_reg;
    uint32_t periph_base; // base address of this UART in peripheral space
    // Thread-safe TX FIFO for outbound bytes (read by DMA)
    std::deque<uint8_t> txFifo;
    std::mutex fifoMutex;
    std::condition_variable fifoCv;
    std::atomic<bool> alive{true};
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    Uart(const char* name);
    ~Uart();

    void SetPeriphBase(uint32_t base) { periph_base = base; }
    uint32_t GetPeriphBase() const { return periph_base; }

    void SetBaudrate(unsigned int br);
    bool IsBusy() const;
    bool HasError() const;
    void Reset();
    void SetData(unsigned char d);
    unsigned char GetData() const;

    // Push a buffer into TX FIFO (notifying any waiters)
    void PushTx(const uint8_t* data, size_t len);
    // Convenience overload for single byte
    void PushTx(uint8_t b);
    // Pop up to len bytes from TX FIFO; blocks until at least 1 byte available or timeout_ms elapsed (0 = wait forever)
    size_t PopTx(uint8_t* out, size_t len, uint32_t timeout_ms = 0);

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
};
