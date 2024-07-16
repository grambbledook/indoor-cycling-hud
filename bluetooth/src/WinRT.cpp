#include "../../include/bluetooth/WinRT.h"

#include <iostream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;

void WinRT::print() {
    std::cout << "Hello from WinRT!" << std::endl;
    winrt::init_apartment(apartment_type::single_threaded);
    std::cout << "BLAHA after init_apartment!" << std::endl;

    // Example: Initialize a Bluetooth LE watcher
    BluetoothLEAdvertisementWatcher watcher;
    watcher.ScanningMode(BluetoothLEScanningMode::Active);

    watcher.Received(&WinRT::process);

    std::cout << "BLAHA before watcher!" << std::endl;
    watcher.Start();
    std::cout << "BLAHA after watcher!" << std::endl;
    // Keep the application running to receive events
    std::cin.get();
    std::cout << "BLAHA after kek-pook!" << std::endl;
    winrt::uninit_apartment();
    std::cout << "BLAHA after uninit_apartment!" << std::endl;
}

void WinRT::process(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs args) {
    // Process received advertisement
    auto btAddress = args.BluetoothAddress();
    wprintf(L"Received advertisement from: %llX\n", btAddress);
}
