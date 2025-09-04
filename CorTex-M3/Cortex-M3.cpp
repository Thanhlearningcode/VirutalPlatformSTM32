#include "Cortex-M3.h"
#include <cstring>
Cortex_M3::Cortex_M3(const char* _name) : Master(_name), \
                                          NAME_CTR::NameController(_name)
{
    this->mName = _name;
}

Cortex_M3::~Cortex_M3()
{
    //Nothing
}

void Cortex_M3::WriteRegister(uint32_t addr, uint32_t data, uint32_t size)
{
    Data_Package package;
    package.Address = addr;
    package.Length  = size;
    package.Buffer  = new uint8_t[size];
    memcpy(package.Buffer,(uint8_t*)&data,size);
    this->Transmit(&package);
}

uint32_t Cortex_M3::ReadRegister(uint32_t addr, uint32_t size)
{
    uint32_t data = 0;
    Data_Package package;
    package.Address = addr;
    package.Length  = size;
    package.Buffer  = new uint8_t[size];
    this->Received(&package);
    memcpy((uint8_t*)&data,package.Buffer,size);
    return data;
}