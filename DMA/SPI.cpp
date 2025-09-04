#include "SPI.h"
#include <memory>


SPI::SPI(const char* name) : Master(name), Slave(name), speed(1000000), mode(0), busy(false), error(false)
{
    this->Name = name;
}

SPI::~SPI() {}

void SPI::SetSpeed(unsigned int s) {
    speed = s;
}

void SPI::SetMode(unsigned int m) {
    mode = m;
}

bool SPI::IsBusy() const {
    return busy;
}

bool SPI::HasError() const {
    return error;
}

void SPI::Reset() {
    busy = false;
    error = false;
}

bool SPI::Transmit(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng truyền dữ liệu
    // ...
    busy = false;
    return true;
}

bool SPI::Received(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng nhận dữ liệu
    // ...
    busy = false;
    return true;
}
