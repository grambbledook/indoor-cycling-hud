

#pragma once
#include <functional>

#include "Data.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth::Advertisement;

class Scanner {
public:
    void scan(const std::function<void(Device)> &receiver);

    void reset_previous_scans();

    void stop() const;

private:
    std::shared_ptr<BluetoothLEAdvertisementWatcher> watcher;
};