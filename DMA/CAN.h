#pragma once

#include "BusInterface.h"
#include "Port.h"
#include <vector>
#include <cstdint>

struct CAN_Frame {
    uint32_t id;
    uint8_t dlc;
    bool extended;
    std::vector<uint8_t> data;
};

class CAN : public Master, public Slave
{
private:
    const char* Name;
    unsigned int baudrate;
    bool busy;
    bool error;
    // Last TX/RX frames
    CAN_Frame tx_frame;
    CAN_Frame rx_frame;
    // Simple acceptance filters (ids allowed)
    std::vector<uint32_t> filters;
public:
    Port<unsigned int> Clock_Port;
    Port<bool> Reset_Port;
public:
    CAN(const char* name);
    ~CAN();

    void SetBaudrate(unsigned int br);
    bool IsBusy() const;
    bool HasError() const;
    void Reset();

    void SetTxFrame(const CAN_Frame &f);
    CAN_Frame GetRxFrame() const;

    void AddFilter(uint32_t id);
    void ClearFilters();
    bool MatchFilter(uint32_t id) const;

    bool Transmit(Data_Package* package) override;
    bool Received(Data_Package* package) override;
};
