#pragma once
#include <string>
#include <vector>

struct Address {
    const std::string value;
};

struct Name {
    const std::string value;
};

struct UUID {
    const std::string value;
    bool operator==(const UUID &other) const;
};

struct GattService {
    const std::string type;
    const UUID service_uuid;
    const UUID characteristic_uuid;
};

struct Device {
    const Name name;
    const Address address;
    const std::vector<GattService> services;
    bool operator==(const Device &other) const;

public:
    [[nodiscard]] std::string deviceId() const;
};

namespace std {
    template<>
    struct hash<UUID> {
        std::size_t operator()(const UUID &uuid) const noexcept {
            return std::hash<std::string>{}(uuid.value);
        }
    };
}

