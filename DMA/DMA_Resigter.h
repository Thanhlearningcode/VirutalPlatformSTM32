#pragma once

#include "Resigter.h"
#include <map>
#include <memory>
#include <functional>

struct DMA_RegisterOffset 
{
    static constexpr uint32_t CCR   = 0x08;
    static constexpr uint32_t CNDTR = 0x0C;
    static constexpr uint32_t CPAR  = 0x10;
    static constexpr uint32_t CMAR  = 0x14;
};

class DMA;
class DMA_Resigter
{
friend class DMA;
private:
    const char* Name;
    std::map<uint32_t, std::unique_ptr<Resigter>> RegMap;
    Resigter* DMA_CCR;
    Resigter* DMA_CMAR;
    Resigter* DMA_CPAR;
    Resigter* DMA_CNDTR;
    std::function<void(uint32_t,uint32_t)> writeCb; // Callback for register writes
public:
    DMA_Resigter (const char* name);
    ~DMA_Resigter();

    uint32_t ReadResigter  (uint32_t offset);
    void     WriteResigter (uint32_t offset, uint32_t value);
    // optional callback invoked after a register write: (offset, value)
    void SetWriteCallback(std::function<void(uint32_t,uint32_t)> cb) { writeCb = cb; }
};