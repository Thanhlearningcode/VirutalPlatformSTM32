#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>
#include <string.h>

constexpr const char* BIT_NAME_DEFAULT = "";

typedef enum 
{
    Bit_Default = 0,
} Bit_State;

typedef enum 
{
    ReadWrite = 0,
    Read,
    Write,
} e_Permit;

class Bit_Config
{
public:
    const char* Bit_Name;
    uint32_t    Start_Addr;
    uint32_t    End_Addr;
    e_Permit    Permit;
    uint32_t    Init_Val;
    uint32_t      Data;

/* Defalt */
Bit_Config ();
Bit_Config (const char* _name, uint32_t start_addr, uint32_t end_addr,
            e_Permit _permit,uint32_t init, uint32_t data);
Bit_Config& operator = (unsigned int val);
operator unsigned int () const;
~Bit_Config ();
};

class Resigter
{
public:
    uint32_t                 Offset;
    const char*              Resigter_Name;
    std::vector<Bit_Config*> Bitset;

    /* Define functions */
    Resigter (const char* _name, uint32_t offset);
    Resigter& operator() (const char* _name, uint32_t start_addr, uint32_t end_addr,
                          e_Permit _permit,uint32_t init, uint32_t data);
    Bit_Config& operator[] (const char* name);
    operator unsigned int () const;
    Resigter& operator = (unsigned int data);
    ~Resigter();
};


