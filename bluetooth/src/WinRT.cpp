#include "WinRT.h"

#include <iostream>

#include "BleClient.h"
#include "BluetoothServices.h"
#include "DeviceRegistry.h"
#include "NotificationService.h"
#include "Scanner.h"
#include "ScannerService.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;

void WinRT::test(const bool multi_threaded) {
    if (multi_threaded) init_apartment(apartment_type::multi_threaded);
    else init_apartment(apartment_type::single_threaded);

    runTest();

    uninit_apartment();

    exit(0);
}

void WinRT::runTest() {
    auto model = std::make_shared<Model>();
    const auto scanner = std::make_shared<ScannerService>(model, Scanner());
    auto registry = std::make_shared<DeviceRegistry>();
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);

    subscribeToNotifications(model->notifications);

    scanner->startScan();
    std::cin.get();

    const auto hrms = model->getDevices(&BLE::Services::HRM);
    if (not hrms.empty()) {
        const auto &device = hrms.front();
        hrm->setDevice(device);
        std::cin.get();
        hrm->unsetDevice(device);
        std::cin.get();
    }

    const auto cscs = model->getDevices(&BLE::Services::CSC);
    if (not cscs.empty()) {
        const auto &device = cscs.front();
        csc->setDevice(device);
        std::cin.get();
        csc->unsetDevice(device);
        std::cin.get();
    }

    const auto pwrs = model->getDevices(&BLE::Services::PWR);
    if (not pwrs.empty()) {
        const auto &device = pwrs.front();
        pwr->setDevice(device);
        std::cin.get();
        pwr->unsetDevice(device);
        std::cin.get();
    }

    const auto fecs = model->getDevices(&BLE::Services::FEC_BIKE_TRAINER);
    if (not fecs.empty()) {
        const auto &device = fecs.front();
        fec->setDevice(device);
        std::cin.get();
        fec->unsetDevice(device);
        std::cin.get();
    }

    std::cin.get();
    registry->stop();
    std::cin.get();
}

void WinRT::subscribeToNotifications(const Notifications &notifications) {
    notifications.measurements.subscribe([](const WorkoutData &data) {
        std::cout << "Received measuremment_update event:" << typeid(data).name() << std::endl;
        std::cout << "  HRM: " << data.hrm.val << ", AVG: " << data.hrm.avg << std::endl;
        std::cout << "  CAD: " << data.cadence.val << ", AVG: " << data.cadence.avg << std::endl;
        std::cout << "  SPD: " << data.speed.val << ", AVG: " << data.speed.avg << std::endl;
        std::cout << "  PWR: " << data.power.val << ", AVG: " << data.power.avg << std::endl;
    });

    notifications.deviceDiscovered.subscribe([](const DeviceDiscovered &event) {
        std::cout << "Device discovered: " << event.device->deviceId() << std::endl;
        std::cout << "  Services: [";
        for (const auto &[type, a, b]: event.device->services) {
            std::cout << type << ", ";
        }
        std::cout << "]" << std::endl;
    });

    notifications.deviceSelected.subscribe([](const DeviceSelected &event) {
        std::cout << "Device selected: " << event.device->deviceId() << std::endl;
        std::cout << " As: " << &event.service << std::endl;
        std::cout << "  Services: [";
        for (const auto &[type, a, b]: event.device->services) {
            std::cout << type << ", ";
        }
        std::cout << "]" << std::endl;
    });
}
