#pragma once
#include <functional>

#include "Data.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth::Advertisement;

class Scanner {
public:
    explicit Scanner(const std::unordered_map<UUID, GattService, UUID::Hash> &supportedGattServices);

    auto startScan(const std::function<void(std::shared_ptr<Device>)> &receiver)  -> void ;

    auto stopScan() const -> void ;

    auto resetPreviousScans() -> void ;

private:
    const std::unordered_map<UUID, GattService, UUID::Hash> supportedGattServices;
    std::shared_ptr<BluetoothLEAdvertisementWatcher> watcher;
};
