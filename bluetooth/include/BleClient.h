#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <memory>
#include <vector>
#include <winrt/Windows.Devices.Bluetooth.h>
#include "Data.h"
#include "WinRtUtils.h"

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

    bool isConnected() const;

    void read(UUID serviceUuid, UUID characteristicUuid);

private:
    void fetchCharacteristics();

private:
    Device device;
    std::shared_ptr<BluetoothLEDevice> connection;
    std::unordered_map<UUID, std::any> characteristics;
};