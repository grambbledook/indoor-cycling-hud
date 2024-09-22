#include "ClientRegistry.h"
#include <unordered_map>
#include <functional>
#include <iostream>
#include <mutex>
#include <memory>
#include <spdlog/spdlog.h>

DeviceRegistry::DeviceRegistry() = default;

DeviceRegistry::~DeviceRegistry() {
    stop(); // Ensure all clients are disconnected and cleaned up on destruction
}

auto DeviceRegistry::connect(const Device &device) -> std::shared_ptr<BleClient> {
    std::lock_guard guard(mutex);

    spdlog::info("DeviceRegistry::connect");

    if (!clients.contains(device.deviceId())) {
        spdlog::info("    Creating new client for device: {}", device.deviceId());
        clients[device.deviceId()] = std::make_shared<BleClient>(device);
        spdlog::info("    Created new client for device: {}", device.deviceId());
    }

    auto client = clients[device.deviceId()];
    spdlog::info("    Checking if client is connected to device: {}", device.deviceId());
    if (!client->isConnected()) {
        spdlog::info("    Connecting to device: {}", device.deviceId());
        client->connect();
    }

    spdlog::info("    Connected to device: {}", device.deviceId());
    return client;
}

auto DeviceRegistry::stop() -> void  {
    std::lock_guard guard(mutex);

    spdlog::info("Stopping all clients.");

    for (const auto &[_, client]: clients) {
        if (client->isConnected()) {
            client->disconnect();
        }
    }
    spdlog::info("All clients stopped.");
}
