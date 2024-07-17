

#pragma once
#include <string>
#include "Data.h"

#include <iomanip>

Address Address::from_uint64_t(const uint64_t &uint) {
    return Address{std::to_string(uint)};
}

Name Name::from_wstring(const std::wstring &wstr) {
    return Name{std::string{wstr.begin(), wstr.end()}};
}

UUID UUID::from_guid(const uint32_t Data1, const uint16_t Data2, const uint16_t Data3, uint8_t Data4[8]) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
            << std::setw(8) << Data1 << '-'
            << std::setw(4) << Data2 << '-'
            << std::setw(4) << Data3 << '-'
            << std::setw(2) << static_cast<int>(Data4[0])
            << std::setw(2) << static_cast<int>(Data4[1]) << '-'
            << std::setw(2) << static_cast<int>(Data4[2])
            << std::setw(2) << static_cast<int>(Data4[3])
            << std::setw(2) << static_cast<int>(Data4[4])
            << std::setw(2) << static_cast<int>(Data4[5])
            << std::setw(2) << static_cast<int>(Data4[6])
            << std::setw(2) << static_cast<int>(Data4[7]);
    return UUID{oss.str()};
}

bool UUID::operator==(const UUID &other) const {
    return value == other.value;
}

