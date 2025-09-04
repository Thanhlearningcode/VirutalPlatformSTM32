#include "SRAM.h"
#include <algorithm>

SRAM::SRAM(size_t bytes) : mem(bytes), sz(bytes) {}

SRAM::~SRAM() {}

bool SRAM::write(uint32_t address, const uint8_t* data, size_t len)
{
    std::lock_guard<std::mutex> g(m);
    if ((uint64_t)address + len > sz) return false;
    std::copy(data, data + len, mem.begin() + address);
    return true;
}

bool SRAM::read(uint32_t address, uint8_t* out, size_t len)
{
    std::lock_guard<std::mutex> g(m);
    if ((uint64_t)address + len > sz) return false;
    std::copy(mem.begin() + address, mem.begin() + address + len, out);
    return true;
}
