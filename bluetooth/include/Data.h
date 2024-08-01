#pragma once

#include <functional>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

struct Address {
    const std::string value;
};

struct Name {
    const std::string value;
};

struct UUID {
    const std::string value;

    bool operator==(const UUID &other) const {
        return value == other.value;
    }

    struct Hash {
        std::size_t operator()(const UUID &uuid) const noexcept {
            return std::hash<std::string>{}(uuid.value);
        }
    };
};

struct GattService {
    const std::string type;
    const UUID service_uuid;
    const UUID characteristic_uuid;

    bool operator==(const GattService &other) const {
        return service_uuid == other.service_uuid;
    }

    struct Hash {
        std::size_t operator()(const GattService &service) const noexcept {
            return UUID::Hash{}(service.service_uuid);
        }
    };
};

struct Device {
    const Name name;
    const Address address;
    const std::unordered_set<GattService, GattService::Hash> services;

    bool operator==(const Device &other) const {
        return deviceId() == other.deviceId() and services == other.services;
    }

    Device operator|(const Device &other) const {
        std::unordered_set<GattService, GattService::Hash> combined = services;
        combined.insert(other.services.begin(), other.services.end());

        return Device(name, address, std::move(combined));
    }

    [[nodiscard]] std::string deviceId() const {
        return name.value + ":" + address.value;
    }
};

enum class Service {
    HEART_RATE,
    CADENCE,
    SPEED,
    POWER,
    BIKE_TRAINER
};

inline std::string asString(const Service &as) {
    switch (as) {
        case Service::HEART_RATE: return "HEART_RATE";
        case Service::CADENCE: return "CADENCE";
        case Service::SPEED: return "SPEED";
        case Service::POWER: return "POWER";
        case Service::BIKE_TRAINER: return "BIKE_TRAINER";
        default: std::unreachable();
    }
}
