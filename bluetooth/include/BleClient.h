#pragma once

#include <any>
#include <expected>
#include <functional>
#include <memory>
#include <vector>
#include <winrt/Windows.Devices.Bluetooth.h>

#include "BleClientEvent.h"
#include "Data.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;

class BleClient {
public:
    explicit BleClient(Device device, const std::function<void(BleClientEvent)> &eventReceiver);

    auto connect() -> void;

    auto subscribe(const UUID &characteristicUuid,
                   const std::function<void(std::shared_ptr<Device>, std::vector<unsigned char>)> &receiver) const -> bool;

    [[nodiscard]] auto unsubscribe(const UUID &characteristicUuid) const -> bool;

    auto disconnect() const -> void;

    [[nodiscard]] auto isConnected() const -> bool;

    [[nodiscard]] auto read(const UUID &characteristicUuid) const -> std::expected<std::vector<unsigned char>, void *>;

private:
    auto fetchCharacteristics() -> void;

    Device device;
    std::shared_ptr<BluetoothLEDevice> connection;
    std::unordered_map<UUID, std::any, UUID::Hash> characteristics;
    std::function<void(BleClientEvent)> eventReceiver;
};
