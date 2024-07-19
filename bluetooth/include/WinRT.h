#pragma once
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

#include "Scanner.h"

using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

class WinRT {
public:
    static void test(bool multi_threaded = false);

    static std::unordered_map<UUID, std::shared_ptr<Device>> do_work();

private:
    static void runTest();
};