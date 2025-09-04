#include "Uart.h"
#include <memory>
#include "../Common/BusInterface/PeripheralRegistry.h"


Uart::Uart(const char* name) : Master(name), Slave(name), baudrate(9600), busy(false), error(false), data_reg(0)
{
    this->Name = name;
    this->periph_base = 0x40000000; // default base
    // register read callback: single register at offset 0 returns data_reg
    PeripheralRegistry::Register(this->periph_base, 4, [this](uint32_t offset, uint8_t* out, size_t len)->bool {
        if (offset == 0 && len > 0) {
            out[0] = this->GetData();
            return true;
        }
        return false;
    });
}

Uart::~Uart() {}

void Uart::SetBaudrate(unsigned int br) {
    baudrate = br;
}

bool Uart::IsBusy() const {
    return busy;
}

bool Uart::HasError() const {
    return error;
}

void Uart::Reset() {
    busy = false;
    error = false;
    data_reg = 0;
}

void Uart::SetData(unsigned char d) {
    data_reg = d;
}

unsigned char Uart::GetData() const {
    return data_reg;
}

bool Uart::Transmit(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng truyền dữ liệu UART
    if (package && package->Length > 0) {
        data_reg = package->Buffer[0];
    }
    busy = false;
    return true;
}

bool Uart::Received(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Mô phỏng nhận dữ liệu UART
    if (package && package->Length > 0) {
        package->Buffer[0] = data_reg;
    }
    busy = false;
    return true;
}
