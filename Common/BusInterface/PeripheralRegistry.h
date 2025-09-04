#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>

namespace PeripheralRegistry {
    using ReadCb = std::function<bool(uint32_t offset, uint8_t* out, size_t len)>;

    // Register a peripheral region [base, base+size) with a read callback.
    bool Register(uint32_t base, size_t size, ReadCb cb);

    // Read len bytes starting at absolute address addr. Returns true if a peripheral handled the read.
    bool Read(uint32_t addr, uint8_t* out, size_t len);
}
