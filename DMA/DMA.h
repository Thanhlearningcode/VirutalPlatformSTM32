#pragma once 

#include "DMA_Resigter.h"
#include "BusInterface.h"
#include "Port.h"

#include "Uart.h"

#include "Timer.h"
#include "I2C.h"
#include "SPI.h"

#include "CAN.h"
// threading and containers
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>

class DMA : public Master , public Slave 
{
private:
    std::unique_ptr<DMA_Resigter> DMA_Reg; 
    const char*                   Name;
    // Simulated SRAM used by DMA
    class SRAM* sram;
    // Forward-declare transfer task
    struct TransferTask {
        uint32_t destAddress; // in SRAM
        std::vector<uint8_t> data;
    };
    // internal queue and synchronization
    std::deque<TransferTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCv;
    std::thread worker;
    std::atomic<bool> stopWorker{false};
public:
    Port<unsigned int>            Clock_Port;
    Port<bool>                    Reset_Port;
public:
    DMA (const char* name);
    ~DMA();

/*Overriding function from slave class*/
    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
    // Enqueue a DMA memory write (user-facing API)
    bool EnqueueToSRAM(uint32_t sramAddress, const uint8_t* data, size_t len);
};