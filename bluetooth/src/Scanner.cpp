#include <functional>

#include "Scanner.h"

#include <iostream>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.Streams.h>

#include "Service.h"
#include "WinRtUtils.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::Advertisement;

void Scanner::start_scan(const std::function<void(Device)> &receiver) {
    reset_previous_scans();

    watcher->Received(
        [receiver](BluetoothLEAdvertisementWatcher const &, BluetoothLEAdvertisementReceivedEventArgs const &args) {
            const auto name = args.Advertisement().LocalName();
            const uint64_t address = args.BluetoothAddress();

            const auto services = args.Advertisement().ServiceUuids();
            const auto services_size = services.Size();

            auto supported_services = std::vector<Service>();
            for (auto i = 0; i < services_size; i++) {
                auto cs = services.GetAt(i);
                auto candidate_service_uuid = WinrtUtils::uuid_from_guid(cs);

                for (const auto &service: Services::SUPPORTED_SERVICES) {
                    if (service.service_uuid == candidate_service_uuid) {
                        supported_services.push_back(service);
                    }
                }
            }

            if (supported_services.empty()) return;

            const auto device = Device{
                WinrtUtils::name_from_hstring(name), WinrtUtils::name_from_hstring(address), supported_services
            };

            receiver(device);
        });

    watcher->Start();
    std::cout << "Scanning for BLE devices...\n";
}

void Scanner::reset_previous_scans() {
    this->stop_scan();
    watcher = std::make_shared<BluetoothLEAdvertisementWatcher>();
}

void Scanner::stop_scan() const {
    if (!watcher) return;

    std::cout << "Stopping scan routine...\n";
    watcher->Stop();
    // uninit_apartment();
}
