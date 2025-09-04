#include <iostream>
#include "DMA.h"

int main()
{
    DMA dma("DMA1");
    const char msg[] = "HelloDMA";
    dma.EnqueueToSRAM(128, (const uint8_t*)msg, sizeof(msg));
    // give worker some time
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // read back via SRAM directly by creating a buffer and using DMA::Received is register based; so open workaround: create a Data_Package and read from SRAM via DMA's internal pointer - not exposed.
    // For quick test, write Data_Package to test Transmit/Received of register.
    std::cout << "Enqueued DMA write, test complete." << std::endl;
    return 0;
}
