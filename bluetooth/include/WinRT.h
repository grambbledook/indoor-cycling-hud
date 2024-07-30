#pragma once
#include <iostream>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

#include "Model.h"
#include "Scanner.h"

using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

class WinRT {
public:
    static void test(bool multi_threaded = false);

    static std::unordered_map<UUID, std::shared_ptr<Device>, UUID::Hash> do_work(const std::shared_ptr<Model> &model);

private:
    static void runTest();

    static void subscribeToNotifications(const Notifications &notifications);
};
