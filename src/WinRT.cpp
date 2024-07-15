#include "WinRT.h"

#include <iostream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;

int main() {
    winrt::init_apartment();

    // Example: Initialize a Bluetooth LE watcher
    BluetoothLEAdvertisementWatcher watcher;
    watcher.ScanningMode(BluetoothLEScanningMode::Active);

    // Subscribe to advertisement received event
    watcher.Received([](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs args) {
        // Process received advertisement
        auto btAddress = args.BluetoothAddress();
        wprintf(L"Received advertisement from: %llX\n", btAddress);
    });

    watcher.Start();

    // Keep the application running to receive events
    std::cin.get();

    winrt::uninit_apartment();
    return 0;
}
