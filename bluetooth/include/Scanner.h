#pragma once
#include <functional>

#include "Data.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth::Advertisement;

class Scanner {
public:
    void start_scan(const std::function<void(std::shared_ptr<Device>)> &receiver);

    void stop_scan() const;

    void reset_previous_scans();

private:
    std::shared_ptr<BluetoothLEAdvertisementWatcher> watcher;
};
