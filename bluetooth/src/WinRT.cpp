#include "WinRT.h"

#include <iostream>

#include "BleClient.h"
#include "Scanner.h"
#include "Service.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;

void WinRT::test(bool multi_threaded) {
    if (multi_threaded) winrt::init_apartment(apartment_type::multi_threaded);
    else winrt::init_apartment(apartment_type::single_threaded);

    runTest();

    uninit_apartment();

    exit(0);
}

void WinRT::runTest() {
    const auto scanner = new Scanner();

    std::cout << "BLAHA kek-pook test one!" << std::endl;

    auto found_devices = std::vector<Device>();
    auto first_receiver = [&found_devices](const Device &device) {
        std::cout << "Received device: " << device.name.value << " with address: " << device.address.value << std::endl;

        std::cout << "   Supported services: [";
        for (const auto &[type, service_uuid, characteristic_uuid]: device.services) {
            std::cout << "{ " << type << ": " << service_uuid.value << "}, ";
        }
        std::cout << "]" << std::endl;

        if (found_devices.empty()) {
            found_devices.push_back(device);
        }
    };
    scanner->start_scan(first_receiver);
    std::cin.get();
    scanner->stop_scan();
    std::cout << "Found devices: " << found_devices.size() << std::endl;

    auto device = found_devices[0];
    auto client = BleClient(device);
    std::cout << "Connecting to device: " << device.name.value << " with address: " << device.address.value <<
            std::endl;

    client.connect();
    std::cin.get();

    std::cout << "Subscribing to HRM service..." << std::endl;
    auto second_receiver = [](const std::vector<uint8_t> &data) {
        std::cout << "Received data: ";
        for (const auto &byte: data) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    };
    const auto result = client.subscribe(Services::HRM.characteristic_uuid, second_receiver);

    if (!result) {
        std::cerr << "Failed to subscribe to HRM service." << std::endl;
        return;
    }
    std::cin.get();

    std::cout << "Disconnecting from device..." << std::endl;
    client.disconnect();

    std::cin.get();
}
