//
// Created by denis on 10/21/2024.
//

#ifndef BLEEVENT_H
#define BLEEVENT_H
#include <memory>

#include "Data.h"

enum class BleConnectionStatus {
    CONNECTED,
    DISCONNECTED
};

class BleClientEvent {
public:
    BleClientEvent(const std::shared_ptr<Device> &device, const BleConnectionStatus &status) : device(device), status(status) {
    }

    std::shared_ptr<Device> device;
    BleConnectionStatus status;
};

#endif /
