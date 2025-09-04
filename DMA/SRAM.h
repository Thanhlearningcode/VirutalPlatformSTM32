#pragma once

#include <vector>
#include <cstdint>
#include <mutex>

class SRAM {
private:
    std::vector<uint8_t> mem;
    std::mutex m;
    size_t sz;
public:
    explicit SRAM(size_t bytes = 64 * 1024);
    ~SRAM();

    // Write len bytes from data into address. Returns false if out-of-range.
    bool write(uint32_t address, const uint8_t* data, size_t len);

    // Read len bytes from address into out buffer. Returns false if out-of-range.
    bool read(uint32_t address, uint8_t* out, size_t len);

    size_t size() const { return sz; }
};
