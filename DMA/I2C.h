#pragma once

#include "BusInterface.h"
#include "Port.h"


class I2C : public Master, public Slave
{
private:
    const char* Name;
    unsigned int speed;
    unsigned int address;
    bool busy;
    bool error;
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    I2C(const char* name);
    ~I2C();

    void SetSpeed(unsigned int s);
    void SetAddress(unsigned int addr);
    bool IsBusy() const;
    bool HasError() const;
    void Reset();

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
};
