#include "Timer.h"
#include <memory>


Timer::Timer(const char* name) : Master(name), Slave(name), value(0), running(false), interrupt(false)
{
    this->Name = name;
}

Timer::~Timer() {}

void Timer::Start() {
    running = true;
}

void Timer::Stop() {
    running = false;
}

void Timer::Reset() {
    value = 0;
    running = false;
    interrupt = false;
}

void Timer::SetValue(unsigned int v) {
    value = v;
}

unsigned int Timer::GetValue() const {
    return value;
}

bool Timer::IsRunning() const {
    return running;
}

bool Timer::HasInterrupt() const {
    return interrupt;
}

bool Timer::Transmit(Data_Package* package)
{
    // Mô phỏng truyền dữ liệu timer
    // ...
    return true;
}

bool Timer::Received(Data_Package* package)
{
    // Mô phỏng nhận dữ liệu timer
    // ...
    return true;
}
