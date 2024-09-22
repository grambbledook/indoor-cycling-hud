#pragma once

#include "Data.h"
#include <winrt/base.h>

namespace WinrtUtils {
    inline auto toGuid(const UUID &uuid) {
        unsigned long Data1;
        uint16_t Data2, Data3;
        std::array<uint8_t, 8> Data4;

        std::istringstream iss(uuid.value);
        char dash;

        if (!(iss >> std::hex >> Data1 >> dash >> Data2 >> dash >> Data3 >> dash)) {
            throw std::invalid_argument("Invalid UUID format");
        }

        for (int i = 0; i < 2; ++i) {
            int byte;
            if (!(iss >> std::hex >> byte)) {
                throw std::invalid_argument("Invalid UUID format");
            }
            Data4[i] = static_cast<uint8_t>(byte);
        }

        if (!(iss >> dash)) {
            throw std::invalid_argument("Invalid UUID format");
        }

        for (int i = 2; i < 8; ++i) {
            int byte;
            if (!(iss >> std::hex >> byte)) {
                throw std::invalid_argument("Invalid UUID format");
            }
            Data4[i] = static_cast<uint8_t>(byte);
        }

        return winrt::guid(Data1, Data2, Data3, Data4);
    }

    inline UUID uuidFromGuid(const winrt::guid &guid) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
                << std::setw(8) << guid.Data1 << '-'
                << std::setw(4) << guid.Data2 << '-'
                << std::setw(4) << guid.Data3 << '-'
                << std::setw(2) << static_cast<int>(guid.Data4[0])
                << std::setw(2) << static_cast<int>(guid.Data4[1]) << '-'
                << std::setw(2) << static_cast<int>(guid.Data4[2])
                << std::setw(2) << static_cast<int>(guid.Data4[3])
                << std::setw(2) << static_cast<int>(guid.Data4[4])
                << std::setw(2) << static_cast<int>(guid.Data4[5])
                << std::setw(2) << static_cast<int>(guid.Data4[6])
                << std::setw(2) << static_cast<int>(guid.Data4[7]);
        return UUID{oss.str()};
    }

    inline auto nameFromHstring(const winrt::hstring &wstr) {
        return Name{std::string{wstr.begin(), wstr.end()}};
    }

    inline auto addressFromLong(const uint64_t &uint) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
                << std::setw(2) << ((uint >> 40) & 0xFF) << ":"
                << std::setw(2) << ((uint >> 32) & 0xFF) << ":"
                << std::setw(2) << ((uint >> 24) & 0xFF) << ":"
                << std::setw(2) << ((uint >> 16) & 0xFF) << ":"
                << std::setw(2) << ((uint >> 8) & 0xFF) << ":"
                << std::setw(2) << (uint & 0xFF);

        return Address{oss.str()};
    }

    inline auto addressToUint64T(const Address &address) {
        uint64_t result = 0;
        std::istringstream iss(address.value);
        std::string byte_str;
        int shift = 40;

        while (std::getline(iss, byte_str, ':')) {
            if (byte_str.size() != 2) {
                throw std::invalid_argument("Invalid Bluetooth address format");
            }
            const uint8_t byte = std::stoul(byte_str, nullptr, 16);
            result |= (static_cast<uint64_t>(byte) << shift);
            shift -= 8;
        }

        return result;
    }
}
