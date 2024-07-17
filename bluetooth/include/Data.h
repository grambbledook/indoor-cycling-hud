#pragma once
#include <sstream>
#include <string>
#include <vector>

struct Address {
    const std::string value;

    static Address from_uint64_t(const uint64_t &uint);
};

struct Name {
    const std::string value;

    static Name from_wstring(const std::wstring &wstr);
};


struct UUID {
    const std::string value;

    static UUID from_guid(const uint32_t Data1, const uint16_t Data2, const uint16_t Data3, uint8_t Data4[8]);

    bool operator==(const UUID& other) const;
};

struct Service {
    const std::string type;
    const UUID service_uuid;
    const UUID characteristic_uuid;
};

struct Device {
    const Name name;
    const Address address;
    const std::vector<Service> services;
};
