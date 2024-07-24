#pragma once
#include <iostream>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

#include "Model.h"
#include "Scanner.h"

using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

class WinRT {
public:
    static void test(bool multi_threaded = false);

    static std::unordered_map<UUID, std::shared_ptr<Device>, UUID::Hash > do_work(const std::shared_ptr<Model> &model);

private:
    static void runTest();

    template<typename T>
    static void subscribeToNotifications(const Notifications<T> &notifications, const GattService &service) {
        notifications.data.subscribe([service](const T &data) {
            std::cout << "Service [" << service.type << "]: Received data of type: " << typeid(data).name() << std::endl;
        });

        notifications.deviceDiscovered.subscribe([service](const std::shared_ptr<Device> &device) {
            std::cout << "Service [" << service.type << "]: Device discovered: " << device->deviceId() << std::endl;
        });

        notifications.deviceSelected.subscribe([service](const std::shared_ptr<Device> &device) {
            std::cout << "Service [" << service.type << "]: Device selected: " << device->deviceId() << std::endl;
        });
    }
};
