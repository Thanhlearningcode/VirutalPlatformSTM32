#include "Resigter.h"

Bit_Config::Bit_Config ()
{
    this->Bit_Name   = BIT_NAME_DEFAULT;
    this->Start_Addr = Bit_Default;
    this->End_Addr   = Bit_Default;
    this->Permit     = e_Permit::ReadWrite;
    this->Init_Val   = Bit_Default;
    this->Data       = Bit_Default;
}

Bit_Config::Bit_Config (const char* _name, uint32_t start_addr, uint32_t end_addr,
                        e_Permit _permit,uint32_t init, uint32_t data)
{
    this->Bit_Name   = _name;
    this->Start_Addr = start_addr;
    this->End_Addr   = end_addr;
    this->Permit     = _permit;
    this->Init_Val   = init;
    this->Data       = data;
}
Resigter::Resigter(const char* name, uint32_t offset)
{
    Resigter_Name = name;
;    Offset = offset;
}
Bit_Config& Bit_Config::operator = (unsigned int val)
{
    uint32_t mask = (uint32_t)((1 << (this->End_Addr - this->Start_Addr + 1)) - 1);
    if ( (this->Permit == e_Permit:: ReadWrite) || (this->Permit == e_Permit::Write) ) 
    {
        this->Data = val &mask;
    }
    return *this;
}

Bit_Config::operator unsigned int () const 
{
    if ( (this->Permit == e_Permit:: ReadWrite) || (this->Permit == e_Permit::Write) ) 
    {
        return this->Data;
    }

    printf("WARNING - This bit register %s is not permit to read\n",this->Bit_Name);
    return 0;
}

Bit_Config::~Bit_Config()
{
    //Nothing
}

Resigter& Resigter::operator() (const char* _name, uint32_t start_addr, uint32_t end_addr,
                                e_Permit _permit,uint32_t init, uint32_t data)
{
    Bitset.push_back(new Bit_Config(_name,start_addr,end_addr,_permit,init,data));
    return *this;
}

Bit_Config& Resigter::operator[] (const char* name)
{
  for (int i=0; i<Bitset.size(); i++)
  {
    if (0 == strcmp(Bitset.at(i)->Bit_Name,name));
    {
        return *(Bitset.at(i));
    }
  }

  printf("ERROR - This bit register name %s is not exist in the register %s\n",name, this->Resigter_Name);
  exit(1);
}

Resigter::operator unsigned int () const
{
    uint32_t data = 0;
    for (size_t i=0;i<Bitset.size();i++)
    {
        uint32_t bit_val = Bitset.at(i)->Data;
        data |= (bit_val << Bitset.at(i)->Start_Addr);
    }
    return data;
}

Resigter& Resigter::operator = (unsigned int data)
{
    for (size_t i=0;i<Bitset.size();i++)
    {
        *(Bitset.at(i)) = data >> Bitset.at(i)->Start_Addr;
    }
    return *this;
}

Resigter::~Resigter()
{
    for (size_t i=0;i<Bitset.size();i++)
    {
        delete Bitset.at(i);
    }
}