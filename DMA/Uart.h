#pragma once

#include "BusInterface.h"
#include "Port.h"


class Uart : public Master, public Slave
{
private:
    const char* Name;
    unsigned int baudrate;
    bool busy;
    bool error;
    unsigned char data_reg;
    uint32_t periph_base; // base address of this UART in peripheral space
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    Uart(const char* name);
    ~Uart();

    void SetPeriphBase(uint32_t base) { periph_base = base; }

    void SetBaudrate(unsigned int br);
    bool IsBusy() const;
    bool HasError() const;
    void Reset();
    void SetData(unsigned char d);
    unsigned char GetData() const;

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
};
