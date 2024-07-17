#include <functional>

#include "Scanner.h"

#include <iostream>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include "Service.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::Advertisement;

void Scanner::scan(const std::function<void(Device)> &receiver) {
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
                auto candidate_service_uuid = UUID::from_guid(cs.Data1, cs.Data2, cs.Data3, cs.Data4);

                for (const auto &service: Services::SUPPORTED_SERVICES) {
                    if (service.service_uuid == candidate_service_uuid) {
                        supported_services.push_back(service);
                    }
                }
            }

            if (supported_services.empty()) return;

            const auto device = Device{
                Name::from_wstring(name.c_str()), Address::from_uint64_t(address), supported_services
            };

            receiver(device);
        });

    watcher->Start();
    std::cout << "Scanning for BLE devices...\n";
}

void Scanner::reset_previous_scans() {
    this->stop();
    watcher = std::make_shared<BluetoothLEAdvertisementWatcher>();
    init_apartment(apartment_type::single_threaded);
}

void Scanner::stop() const {
    if (!watcher) return;

    std::cout << "Stopping scan routine...\n";
    watcher->Stop();
    uninit_apartment();
}