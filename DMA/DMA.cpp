#include "DMA.h"
#include <memory>
#include <thread>
#include <deque>
#include <condition_variable>
#include <atomic>
#include "SRAM.h"
#include "../Common/BusInterface/PeripheralRegistry.h"

#include "Timer.h"
#include "I2C.h"
#include "SPI.h"

DMA::DMA(const char* name) : Master(name), Slave(name)
{
    std::string reg_name(name);
    this->Name = name;
    DMA_Reg = std::make_unique<DMA_Resigter>((reg_name + "_register").c_str());
    // create simulated SRAM (64KB default)
    sram = new SRAM(64 * 1024);

    // start worker thread
    worker = std::thread([this]() {
        while (!stopWorker.load()) {
            TransferTask task;
            {
                std::unique_lock<std::mutex> lk(this->queueMutex);
                this->queueCv.wait(lk, [this]() { return !this->taskQueue.empty() || stopWorker.load(); });
                if (stopWorker.load() && this->taskQueue.empty()) break;
                task = std::move(this->taskQueue.front());
                this->taskQueue.pop_front();
            }
            // perform write to SRAM
            if (!sram->write(task.destAddress, task.data.data(), task.data.size())) {
                printf("DMA worker: SRAM write out of range at 0x%08x len=%zu\n", task.destAddress, task.data.size());
            } else {
                // simulate some transfer latency
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    });

    // register write callback -> monitor CCR.EN
    DMA_Reg->SetWriteCallback([this](uint32_t offset, uint32_t val){
        // If CCR written, check enable bit (bit 0)
        if (offset == DMA_RegisterOffset::CCR) {
            if (val & 0x1) {
                // start transfer: read CMAR (memory addr), CPAR (periph addr), CNDTR (count)
                uint32_t memAddr = DMA_Reg->ReadResigter(DMA_RegisterOffset::CMAR);
                uint32_t periphAddr = DMA_Reg->ReadResigter(DMA_RegisterOffset::CPAR);
                uint32_t count = DMA_Reg->ReadResigter(DMA_RegisterOffset::CNDTR);
                // For this simple model, assume periphAddr is pointer to a buffer we can't access; instead create dummy data pattern
                std::vector<uint8_t> data;
                data.resize(count);
                bool periph_read_ok = false;
                // try to read from registered peripheral
                periph_read_ok = PeripheralRegistry::Read(periphAddr, data.data(), count);
                if (!periph_read_ok) {
                    // fallback: fill with pattern
                    for (uint32_t i = 0; i < count; ++i) data[i] = (uint8_t)((periphAddr + i) & 0xFF);
                }
                // enqueue to SRAM at memAddr
                this->EnqueueToSRAM(memAddr, data.data(), data.size());
            }
        }
    });
}

DMA::~DMA()
{
    // stop worker
    stopWorker.store(true);
    queueCv.notify_all();
    if (worker.joinable()) worker.join();
    delete sram;
}

bool DMA::Transmit(Data_Package* package)
{
    uint32_t data = 0;
    memcpy((uint8_t*)&data,package->Buffer,package->Length);
    DMA_Reg->WriteResigter(package->Address,data);
    return true;
}

bool DMA::Received(Data_Package* package)
{
    uint32_t data = 0;
    data = DMA_Reg->ReadResigter(package->Address);
    memcpy(package->Buffer,(uint8_t*)&data,package->Length);
    return true;
}

bool DMA::EnqueueToSRAM(uint32_t sramAddress, const uint8_t* data, size_t len)
{
    if (data == nullptr || len == 0) return false;
    TransferTask t;
    t.destAddress = sramAddress;
    t.data.assign(data, data + len);
    {
        std::lock_guard<std::mutex> g(queueMutex);
        taskQueue.push_back(std::move(t));
    }
    queueCv.notify_one();
    return true;
}


