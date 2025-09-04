#include "PeripheralRegistry.h"
#include <map>
#include <mutex>

static std::map<uint32_t, std::pair<size_t, PeripheralRegistry::ReadCb>> g_registry;
static std::mutex g_reg_mtx;

namespace PeripheralRegistry {

bool Register(uint32_t base, size_t size, ReadCb cb)
{
    std::lock_guard<std::mutex> g(g_reg_mtx);
    if (!cb) return false;
    g_registry[base] = std::make_pair(size, cb);
    return true;
}

bool Read(uint32_t addr, uint8_t* out, size_t len)
{
    std::lock_guard<std::mutex> g(g_reg_mtx);
    for (auto &p : g_registry) {
        uint32_t base = p.first;
        size_t sz = p.second.first;
    if (addr >= base && (addr + len) <= (base + sz)) {
        printf("PR: Read hit base=0x%08x addr=0x%08x len=%zu\n", base, addr, len);
            return p.second.second(addr - base, out, len);
        }
    }
    printf("PR: Read miss addr=0x%08x len=%zu\n", addr, len);
    return false;
}

}
