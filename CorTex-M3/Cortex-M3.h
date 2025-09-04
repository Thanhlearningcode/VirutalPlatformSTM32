#pragma once 
#include <iostream>
#include "BusInterface.h"
#include "NameController.h"

class Cortex_M3 : public Master , public NAME_CTR::NameController
{
private:
    const char* mName;
public:
    Cortex_M3(const char* _name);
    ~Cortex_M3();

    /* APIS */
    void WriteRegister(uint32_t addr, uint32_t data, uint32_t size);
    uint32_t ReadRegister(uint32_t addr, uint32_t size);
};