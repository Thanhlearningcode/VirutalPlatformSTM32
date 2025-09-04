#pragma once

#include "BusInterface.h"
#include "Port.h"


class SPI : public Master, public Slave
{
private:
    const char* Name;
    unsigned int speed;
    unsigned int mode;
    bool busy;
    bool error;
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    SPI(const char* name);
    ~SPI();

    void SetSpeed(unsigned int s);
    void SetMode(unsigned int m);
    bool IsBusy() const;
    bool HasError() const;
    void Reset();

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
};
