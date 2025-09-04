#include <iostream>
#include "Port.h"
#include <memory>

int main (int argc, char const* argv[])
{
    auto signal_clock = std::make_shared<Signal<Type_Clock>>(CLOCK_16MHZ);
    auto clock_osc    = std::make_unique<Port<Type_Clock>>();  
    auto clock        = std::make_unique<Port<Type_Clock>>(); 

    (*clock_osc)(*signal_clock);
    (*clock)(*signal_clock);

    printf("Read clock Port clk      : %d\n", clock->Read_Data());
    printf("Read clock Port clk_osc  : %d\n", clock_osc->Read_Data());

    return 0;
}
