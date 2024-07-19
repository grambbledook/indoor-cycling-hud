#include "WinRT.h"

#include <iostream>

#include "BleClient.h"
#include "DeviceRegistry.h"
#include "NotificationService.h"
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
    else init_apartment(apartment_type::single_threaded);

    runTest();

    uninit_apartment();

    exit(0);
}

std::vector<Device> WinRT::do_work() {
    const auto scanner = new Scanner();
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

    return found_devices;
}

void WinRT::runTest() {
    const auto found_devices = do_work();
    std::cout << "Found devices: " << found_devices.size() << std::endl;

    const auto &device = found_devices[0];

    auto registry = std::make_shared<DeviceRegistry>();
    const auto service = std::make_shared<HrmNotificationService>(registry);

    service->set_device(std::make_shared<Device>(device));
    std::cin.get();

    registry->stop();
    std::cin.get();
}
