#pragma once

#include <unordered_map>
#include <mutex>
#include <memory>
#include "BleClient.h" // Assuming this is the class similar to the Python BleClient
#include "EventBus.h"

class DeviceRegistry {
    std::unordered_map<std::string, std::shared_ptr<BleClient> > clients;
    std::mutex mutex;
    std::shared_ptr<EventBus> eventBus;

public:
    explicit DeviceRegistry(const std::shared_ptr<EventBus> &eventBus);

    ~DeviceRegistry();

    auto connect(const Device &device) -> std::shared_ptr<BleClient>;

    auto stop() -> void;
};
