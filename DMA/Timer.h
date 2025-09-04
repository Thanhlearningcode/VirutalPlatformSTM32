#pragma once

#include "BusInterface.h"
#include "Port.h"


class Timer : public Master, public Slave
{
private:
    const char* Name;
    unsigned int value;
    bool running;
    bool interrupt;
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    Timer(const char* name);
    ~Timer();

    void Start();
    void Stop();
    void Reset();
    void SetValue(unsigned int v);
    unsigned int GetValue() const;
    bool IsRunning() const;
    bool HasInterrupt() const;

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);
};
