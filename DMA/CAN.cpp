#include "CAN.h"
#include <memory>
#include <cstring>

CAN::CAN(const char* name) : Master(name), Slave(name), baudrate(500000), busy(false), error(false)
{
    this->Name = name;
    tx_frame = {};
    rx_frame = {};
}

CAN::~CAN() {}

void CAN::SetBaudrate(unsigned int br) {
    baudrate = br;
}

bool CAN::IsBusy() const {
    return busy;
}

bool CAN::HasError() const {
    return error;
}

void CAN::Reset() {
    busy = false;
    error = false;
    tx_frame = {};
    rx_frame = {};
    filters.clear();
}

void CAN::SetTxFrame(const CAN_Frame &f) {
    tx_frame = f;
}

CAN_Frame CAN::GetRxFrame() const {
    return rx_frame;
}

void CAN::AddFilter(uint32_t id) {
    filters.push_back(id);
}

void CAN::ClearFilters() {
    filters.clear();
}

bool CAN::MatchFilter(uint32_t id) const {
    if (filters.empty()) return true; // accept all when no filter
    for (auto &fid : filters) if (fid == id) return true;
    return false;
}

bool CAN::Transmit(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // If Data_Package contains serialized CAN_Frame, deserialize minimal fields
    if (package && package->Length > 0) {
        // Expect at least 6 bytes: id(4), dlc(1), extended(1)
        if (package->Length >= 6) {
            uint32_t id = 0;
            std::memcpy(&id, package->Buffer, 4);
            uint8_t dlc = package->Buffer[4];
            uint8_t ext = package->Buffer[5];
            tx_frame.id = id;
            tx_frame.dlc = dlc;
            tx_frame.extended = ext != 0;
            tx_frame.data.assign(package->Buffer + 6, package->Buffer + 6 + dlc);
        }
    }
    busy = false;
    return true;
}

bool CAN::Received(Data_Package* package)
{
    if (busy) { error = true; return false; }
    busy = true;
    // Prepare rx_frame into package if frame matches filter
    if (!MatchFilter(rx_frame.id)) { busy = false; return false; }
    if (package && package->Length > 0) {
        // serialize rx_frame: id(4), dlc(1), extended(1), data...
        size_t need = 6 + rx_frame.data.size();
        if (package->Length >= (int)need) {
            std::memcpy(package->Buffer, &rx_frame.id, 4);
            package->Buffer[4] = rx_frame.dlc;
            package->Buffer[5] = rx_frame.extended ? 1 : 0;
            if (!rx_frame.data.empty()) std::memcpy(package->Buffer + 6, rx_frame.data.data(), rx_frame.data.size());
        }
    }
    busy = false;
    return true;
}
