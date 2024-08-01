#pragma once

#include <any>
#include <functional>
#include <memory>
#include <vector>
#include <winrt/Windows.Devices.Bluetooth.h>
#include "Data.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;

class BleClient {
public:
    explicit BleClient(Device device);

    void connect();

    bool subscribe(const UUID &characteristicUuid,
                   const std::function<void(std::shared_ptr<Device>, std::vector<uint8_t>)> &receiver) const;

    bool unsubscribe(const UUID &characteristicUuid) const;

    void disconnect() const;

    [[nodiscard]] bool isConnected() const;

    [[nodiscard]] std::pair<std::vector<uint8_t>, bool> read(const UUID &characteristicUuid) const;

private:
    void fetchCharacteristics();

private:
    Device device;
    std::shared_ptr<BluetoothLEDevice> connection;
    std::unordered_map<UUID, std::any, UUID::Hash> characteristics;
};
