#pragma once

#include <iostream>
#include <stdint.h>

typedef enum 
{
    CLOCK_16MHZ  = 16000,
    CLOCK_72MHZ  = 72000,
    CLOCK_100MHZ = 100000,
} Clock_Mhz;

using Type_Clock = unsigned int;
/* Create Signal */
template<class Type>
class Port;
template<class Type = bool>
class Signal
{
friend class Port<Type>;
private:
    Type* mData;
public:
    Signal(Type data)
    {
        this->mData = new Type(data);
    }
    ~Signal()
    {
        delete this->mData;
    }
};

/* Define class Port */
template<class Type = bool>
class Port
{
private:
    Type* mData;
public:
    Port()
    {
        mData = NULL;
    }
    ~Port()
    {
        // Nothing
    }

    Port& operator()(Signal<Type>& obj)
    {   
        this->mData = obj.mData;
        return *this; 
    }

    Type Read_Data() const
    {
        if (this->mData == NULL)
        {
            printf("Error- Port is not found\n");
            return false;
        }
        return *(this->mData);
    }

    Port& operator = (Type val)
    {
        if (this->mData == NULL)
        {
            printf("Error - Port is not found\n");
            return false;
        }
        *(this->mData) = val;
        return *this;
    }
};


