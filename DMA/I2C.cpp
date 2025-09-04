#include "I2C.h"
#include <memory>


I2C::I2C(const char* name) : Master(name), Slave(name), speed(100000), address(0x00), busy(false), error(false)
{
    this->Name = name;
}

I2C::~I2C() {}

void I2C::SetSpeed(unsigned int s) {
    speed = s;
}

void I2C::SetAddress(unsigned int addr) {
    address = addr;
}

bool I2C::IsBusy() const {
    return busy;
}

bool I2C::HasError() const {
    return error;
}

void I2C::Reset() {
    busy = false;
    error = false;
}

bool I2C::Transmit(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng truyền dữ liệu
    // ...
    busy = false;
    return true;
}

bool I2C::Received(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng nhận dữ liệu
    // ...
    busy = false;
    return true;
}
