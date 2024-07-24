#include "WinRT.h"

#include <iostream>

#include "BleClient.h"
#include "DeviceRegistry.h"
#include "NotificationService.h"
#include "Scanner.h"
#include "ScannerService.h"
#include "Service.h"

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
    auto scanner = std::make_shared<ScanneService>(model, Scanner());
    auto registry = std::make_shared<DeviceRegistry>();
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);

    subscribeToNotifications(model->hrmNotifications, Services::HRM);
    subscribeToNotifications(model->cadenceNotifications, Services::CSC);
    subscribeToNotifications(model->speedNotifications, Services::PWR);
    subscribeToNotifications(model->powerNotifications, Services::FEC_BIKE_TRAINER);

    scanner->startScan();
    std::cin.get();

    std::cout << "Found devices: " << model->devices.size() << std::endl;
    for (auto const &[uuid, device]: model->devices) {
        std::cout << "  Found device: " << uuid << std::endl;
    }

    const auto hrms = model->getDevices(Services::HRM);
    if (not hrms.empty()) {
        const auto &device = hrms.front();
        hrm->set_device(device);
        std::cin.get();
        hrm->unset_device(device);
        std::cin.get();
    }

    const auto cscs = model->getDevices(Services::CSC);
    if (not cscs.empty()) {
        const auto &device = cscs.front();
        csc->set_device(device);
        std::cin.get();
        csc->unset_device(device);
        std::cin.get();
    }

    const auto pwrs = model->getDevices(Services::PWR);
    if (not pwrs.empty()) {
        const auto &device = pwrs.front();
        pwr->set_device(device);
        std::cin.get();
        pwr->unset_device(device);
        std::cin.get();
    }

    const auto fecs = model->getDevices(Services::FEC_BIKE_TRAINER);
    if (not fecs.empty()) {
        const auto &device = fecs.front();
        fec->set_device(device);
        std::cin.get();
        fec->unset_device(device);
        std::cin.get();
    }

    std::cin.get();
    registry->stop();
    std::cin.get();
}
