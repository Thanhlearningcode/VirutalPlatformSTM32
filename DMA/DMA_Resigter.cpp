#include "DMA_Resigter.h"

DMA_Resigter::DMA_Resigter (const char* _name) 
{
    this->Name = _name;

    auto DMA_CCR = std::make_unique<Resigter>("DMA_CCR", DMA_RegisterOffset::CCR);
    (*DMA_CCR)("EN",      0,  0,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("TCIE",    1,  1,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("HTIE",    2,  2,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("TEIE",    3,  3,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("DIR",     4,  4,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("CIRC",    5,  5,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("PINC",    6,  6,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("MINC",    7,  7,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("PSIZE",   8,  9,  e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("MSIZE",   10, 11, e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("PL",      12, 13, e_Permit::ReadWrite, 0, 0);
    (*DMA_CCR)("MEM2MEM", 14, 14, e_Permit::ReadWrite, 0, 0);
    RegMap[DMA_RegisterOffset::CCR] = std::move(DMA_CCR);

    auto DMA_CMAR = std::make_unique<Resigter>("DMA_CMAR", DMA_RegisterOffset::CMAR);
    (*DMA_CMAR)("MA", 0, 31, e_Permit::ReadWrite, 0, 0);
    RegMap[DMA_RegisterOffset::CMAR] = std::move(DMA_CMAR);

    auto DMA_CPAR = std::make_unique<Resigter>("DMA_CPAR", DMA_RegisterOffset::CPAR);
    (*DMA_CPAR)("PA", 0, 31, e_Permit::ReadWrite, 0, 0);
    RegMap[DMA_RegisterOffset::CPAR] = std::move(DMA_CPAR);

    auto DMA_CNDTR = std::make_unique<Resigter>("DMA_CNDTR", DMA_RegisterOffset::CNDTR);
    (*DMA_CNDTR)("NDT", 0, 16, e_Permit::ReadWrite, 0, 0);
    RegMap[DMA_RegisterOffset::CNDTR] = std::move(DMA_CNDTR);
}

DMA_Resigter::~DMA_Resigter() 
{
    // Nothing
}

uint32_t DMA_Resigter::ReadResigter(uint32_t offset)
{
    auto it = RegMap.find(offset);
    if (it != RegMap.end()) 
    {
        return *(it->second);
    }
    printf("WARNING - The offset 0x%.8x is not exist on register list of STM32 DMA\n", offset);
    return 0;
}

void DMA_Resigter::WriteResigter(uint32_t offset, uint32_t val)
{
    auto it = RegMap.find(offset);
    if (it != RegMap.end()) 
    {
        *(it->second) = val;
    if (writeCb) writeCb(offset, val);
    } 
    else
    {
        printf("WARNING - The offset 0x%.8x is not exist on register list of STM32 DMA\n", offset);
    }
}