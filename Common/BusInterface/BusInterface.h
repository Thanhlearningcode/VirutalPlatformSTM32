#pragma once

#include <iostream>
#include <vector>
class Data_Package 
{
public:
    uint32_t Address;
    uint32_t Length;
    uint8_t* Buffer;
    Data_Package();
};

/* Define Class Slave */
class Slave 
{
private:
    const char *m_Name;
public:
    Slave(const char* Name);
    Slave() = delete;
    ~Slave();

    /*Pure virutal function*/
    virtual bool Transmit(Data_Package* package) = 0;
    virtual bool Received(Data_Package* package) = 0;
};

/* Define Class Master */
class Master 
{
private:
    const char* m_Name;
    std::vector<Slave*> m_SlaveSet; //< Help to upgraded model than using array
public:
    Master(const char* Name);
    Master() = delete;
    ~Master();

    bool Transmit(Data_Package* package);
    bool Received(Data_Package* package);

    Master& operator()(Slave &obj); 
};
