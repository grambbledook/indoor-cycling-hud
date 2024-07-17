
#include "WinRT.h"

#include <iostream>

#include "Scanner.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;

void WinRT::print() {
    const auto scanner = new Scanner();

    std::cout << "BLAHA kek-pook test one!" << std::endl;
    auto first_receiver = [](Device device) {
        std::cout << "Received device: " << device.name.value << " with address: " << device.address.value << std::endl;
        std::cout << "   Supported services: [";
        for (const auto &service: device.services) {
            std::cout << "{ " << service.type << ": " << service.service_uuid.value << "}, ";
        }
        std::cout << "]" << std::endl;
    };
    scanner->scan(first_receiver);
    std::cin.get();
    scanner->stop();

    std::cin.get();
    std::cout << "BLAHA kek-pook test two!" << std::endl;

    auto second_receiver = [](Device device) {
        std::cout << "Huiceived device: " << device.name.value << " with address: " << device.address.value <<
                std::endl;
    };
    scanner->scan(second_receiver);
    std::cin.get();
    scanner->stop();
    std::cout << "BLAHA after uninit_apartment!" << std::endl;
}
