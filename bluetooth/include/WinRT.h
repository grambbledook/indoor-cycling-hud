#pragma once
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

class WinRT {
public:
    static void test(bool multi_threaded = false);
private:
    static void runTest();
};