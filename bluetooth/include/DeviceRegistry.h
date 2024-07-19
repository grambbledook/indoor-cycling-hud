#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>
#include "BleClient.h" // Assuming this is the class similar to the Python BleClient

class DeviceRegistry {
private:
    std::unordered_map<std::string, std::shared_ptr<BleClient>> clients;
    std::mutex mutex;

public:
    DeviceRegistry();
    ~DeviceRegistry();

    std::shared_ptr<BleClient> connect(const Device& device);
    void stop();
};