#pragma once

#include <unordered_map>
#include <mutex>
#include <memory>
#include "BleClient.h" // Assuming this is the class similar to the Python BleClient

class DeviceRegistry {
    std::unordered_map<std::string, std::shared_ptr<BleClient> > clients;
    std::mutex mutex;

public:
    DeviceRegistry();

    ~DeviceRegistry();

    auto connect(const Device &device) -> std::shared_ptr<BleClient>;

    auto stop() -> void;
};
