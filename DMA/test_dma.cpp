#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
using namespace std::chrono_literals;

#include "DMA.h"
#include "Uart.h"

int main()
{
    // Create UART and DMA
    Uart uart("UART1");
    DMA dma("DMA1");

    // Prepare a message to send through UART FIFO
    const char* text = "Hello from UART to DMA";
    const size_t N = std::strlen(text);
    // Push data into UART TX FIFO (thread-safe)
    uart.PushTx(reinterpret_cast<const uint8_t*>(text), N);

    // Program DMA registers to transfer from UART peripheral base to SRAM[128]
    std::cout << "Write CMAR=128" << std::endl;
    dma.WriteReg(DMA_RegisterOffset::CMAR, 128);
    std::cout << "Write CPAR=0x40000000" << std::endl;
    dma.WriteReg(DMA_RegisterOffset::CPAR, 0x40000000);
    std::cout << "Write CNDTR=" << N << std::endl;
    dma.WriteReg(DMA_RegisterOffset::CNDTR, static_cast<uint32_t>(N));
    std::cout << "Write CCR.EN=1" << std::endl;
    dma.WriteReg(DMA_RegisterOffset::CCR, 0x1);

    // Wait a bit to let worker handle transfer
    std::this_thread::sleep_for(200ms);

    // Fallback: also enqueue directly to test worker and SRAM path
    // (uncomment if diagnosing peripheral read path)
    // dma.EnqueueToSRAM(128, reinterpret_cast<const uint8_t*>(text), N);
    // std::this_thread::sleep_for(50ms);

    // Read back from SRAM and compare
    std::vector<uint8_t> out(N);
    bool ok = dma.ReadSRAM(128, out.data(), out.size());
    if (!ok) {
        std::cerr << "SRAM read failed" << std::endl;
        return 1;
    }
    // Print hex dump
    std::cout << "SRAM dump: ";
    for (auto b : out) {
        std::cout << std::hex << std::uppercase << (int)(b & 0xFF) << ' ';
    }
    std::cout << std::dec << "\n";
    if (std::memcmp(out.data(), text, N) == 0) {
        std::cout << "PASS\n";
        return 0;
    } else {
        std::cout << "FAIL\n";
        return 2;
    }
}
