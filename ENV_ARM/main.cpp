#include "DMA.h"
#include "DMA_Resigter.h"
#include <iostream>
#include <iomanip>
#include <memory>

class CPU : public Master 
{
public:
    CPU(const char* name) : Master(name) {}
};

int main(int argc, char* argv[]) 
{
    std::unique_ptr<DMA> dma = std::make_unique<DMA>("DMA");
    std::unique_ptr<CPU> cpu = std::make_unique<CPU>("CPU");

    (*cpu)(*dma);

    Data_Package data;
    data.Address = 0x08;
    data.Length = 4;
    data.Buffer = new uint8_t[4];

    data.Buffer[0] = 0xA1;
    data.Buffer[1] = 0xB2;
    data.Buffer[2] = 0xC3;
    data.Buffer[3] = 0xD4;

    std::cout << "Sending data to DMA...\n";
    cpu->Transmit(&data);

    std::cout << "Data buffer sent: ";
    for (unsigned int i = 0; i < data.Length; ++i) 
    {
        std::cout << "0x"
                  << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(data.Buffer[i])
                  << " ";
    }
    std::cout << std::dec << std::endl;

    Data_Package received;
    received.Address = data.Address;
    received.Length  = data.Length;
    received.Buffer  = new uint8_t[received.Length];

    dma->Received(&received);  // đọc lại từ thanh ghi

    std::cout << "Data received from DMA register: ";
    for (unsigned int i = 0; i < received.Length; ++i) 
    {
        std::cout << "0x"
                  << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(received.Buffer[i])
                  << " ";
    }
    std::cout << std::dec << std::endl;

    
    delete[] data.Buffer;
    delete[] received.Buffer;


    return 0;
}
