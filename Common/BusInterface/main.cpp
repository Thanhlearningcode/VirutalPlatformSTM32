#include <iostream>
#include <memory>
#include "BusInterface.h"

class CPU : public Master
{
public:
    CPU(const char* Name) : Master(Name) {;}
};

class Register : public Slave
{
public:
    Register(const char* Name) : Slave(Name) {;}

    /*Define Pure virutal function*/
    bool Transmit(Data_Package* package)
    {
        printf ("This is received Data\n");
        return true;
    }
    bool Received(Data_Package* package)
    {
        printf ("This is received Data\n");
        return true;
    }
};

int main (int argc, char const* argv[])
{
    std::unique_ptr<CPU> core_m3 = std::make_unique<CPU>("ARM");
    std::shared_ptr<Register> reg     = std::make_shared<Register>("RegSTM");
    (*core_m3)(*reg); //< connect
    Data_Package data;
    core_m3->Transmit(&data);
    printf("Thanhdeptrai\n");

    return 0;
}