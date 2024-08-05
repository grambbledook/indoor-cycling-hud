#include "ClientRegistry.h"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <mutex>
#include <memory>

DeviceRegistry::DeviceRegistry() = default;

DeviceRegistry::~DeviceRegistry() {
    stop(); // Ensure all clients are disconnected and cleaned up on destruction
}

std::shared_ptr<BleClient> DeviceRegistry::connect(const Device &device) {
    std::cout << "DeviceRegistry::connect" << std::endl;
    std::lock_guard guard(mutex);
    std::cout << "    lock acquired" << std::endl;

    if (!clients.contains(device.deviceId())) {
        std::cout << "    Creating new client for device: " << device.deviceId() << std::endl;
        clients[device.deviceId()] = std::make_shared<BleClient>(device);
        std::cout << "    Created new client for device: " << device.deviceId() << std::endl;
    }

    auto client = clients[device.deviceId()];
    std::cout << "    Checking if client is connected to device: " << device.deviceId() << std::endl;
    if (!client->isConnected()) {
        std::cout << "    Connecting to device: " << device.deviceId() << std::endl;
        client->connect();
    }

    std::cout << "    Connected to device: " << device.deviceId() << std::endl;
    return client;
}

void DeviceRegistry::stop() {
    std::lock_guard guard(mutex);
    std::cout << "Stopping all clients." << std::endl;

    for (const auto &[_, client]: clients) {
        client->disconnect();
    }
    std::cout << "All clients stopped." << std::endl;
}
