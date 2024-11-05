#pragma once

#include <functional>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

struct Address {
    const std::string value;

    auto operator==(const Address &other) const {
        return value == other.value;
    }
};

struct Name {
    const std::string value;

    auto operator==(const Name &other) const {
        return value == other.value;
    }
};

struct UUID {
    const std::string value;

    auto operator==(const UUID &other) const {
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

    auto operator==(const GattService &other) const {
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

    auto operator==(const Device &other) const -> bool {
        return deviceId() == other.deviceId() && services == other.services;
    }

    auto operator|(const Device &other) const {
        std::unordered_set<GattService, GattService::Hash> combined = services;
        combined.insert(other.services.begin(), other.services.end());

        return Device(name, address, std::move(combined));
    }

    [[nodiscard]] auto deviceId() const -> std::string {
        return name.value + ":" + address.value;
    }
};

inline auto fromDeviceId(const std::string &deviceId) -> std::shared_ptr<Device> {
    const auto separator = deviceId.find(':');
    const auto name = deviceId.substr(0, separator);
    const auto address = deviceId.substr(separator + 1);

    return std::make_shared<Device>(Device{Name{name}, Address{address}, {}});
}
