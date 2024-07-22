#pragma once
#include <string>
#include "Data.h"


bool UUID::operator==(const UUID &other) const {
    return value == other.value;
}

bool Device::operator==(const Device &other) const {
    return deviceId() == other.deviceId();
}

std::string Device::deviceId() const {
    return name.value + ":" + address.value;
}
