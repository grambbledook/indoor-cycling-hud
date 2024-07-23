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

std::unordered_map<UUID, std::shared_ptr<Device> > WinRT::do_work() {
    const auto scanner = new Scanner();
    auto found_devices = std::unordered_map<UUID, std::shared_ptr<Device> >();

    auto first_receiver = [&found_devices](const Device &device) {
        std::cout << "Received device: " << device.name.value << " with address: " << device.address.value << std::endl;

        std::cout << "   Supported services: [";
        for (const auto &[type, service_uuid, characteristic_uuid]: device.services) {
            std::cout << "{ " << type << ": " << service_uuid.value << "}, ";
        }
        std::cout << "]" << std::endl;

        bool hasHRMService = false;
        bool hasCSCService = false;
        bool hasPWRService = false;
        bool hasFECService = false;

        for (const auto &[type, service_uuid, characteristic_uuid]: device.services) {
            if (service_uuid == Services::HRM.service_uuid) {
                hasHRMService = true;
            }
            if (service_uuid == Services::CSC.service_uuid) {
                hasCSCService = true;
            }
            if (service_uuid == Services::PWR.service_uuid) {
                hasPWRService = true;
            }
            if (service_uuid == Services::FEC_BIKE_TRAINER.service_uuid) {
                hasFECService = true;
            }
        }

        if (hasHRMService && !found_devices.contains(Services::HRM.service_uuid)) {
            found_devices[Services::HRM.service_uuid] = std::make_shared<Device>(device);
        }

        if (hasCSCService && !found_devices.contains(Services::CSC.service_uuid)) {
            found_devices[Services::CSC.service_uuid] = std::make_shared<Device>(device);
        }

        if (hasPWRService && !found_devices.contains(Services::PWR.service_uuid)) {
            found_devices[Services::PWR.service_uuid] = std::make_shared<Device>(device);
        }

        if (hasFECService && !found_devices.contains(Services::FEC_BIKE_TRAINER.service_uuid)) {
            found_devices[Services::FEC_BIKE_TRAINER.service_uuid] = std::make_shared<Device>(device);
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

    auto model = std::make_shared<Model>();
    auto registry = std::make_shared<DeviceRegistry>();
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);

    // if (found_devices.contains(Services::HRM.service_uuid)) {
    //     const auto &device = found_devices.at(Services::HRM.service_uuid);
    //     hrm->set_device(device);
    //     std::cin.get();
    //     hrm->unset_device(device);
    //     std::cin.get();
    // }
    //
    // if (found_devices.contains(Services::CSC.service_uuid)) {
    //     const auto &device = found_devices.at(Services::CSC.service_uuid);
    //     csc->set_device(device);
    //     std::cin.get();
    //     csc->unset_device(device);
    //     std::cin.get();
    // }
    //
    // if (found_devices.contains(Services::PWR.service_uuid)) {
    //     const auto &device = found_devices.at(Services::PWR.service_uuid);
    //     pwr->set_device(device);
    //     std::cin.get();
    //     pwr->unset_device(device);
    //     std::cin.get();
    // }

    if (found_devices.contains(Services::FEC_BIKE_TRAINER.service_uuid)) {
        const auto &device = found_devices.at(Services::FEC_BIKE_TRAINER.service_uuid);
        fec->set_device(device);
        std::cin.get();
        fec->unset_device(device);
        std::cin.get();
    }

    std::cin.get();
    registry->stop();
    std::cin.get();
}
