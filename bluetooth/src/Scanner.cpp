#include <functional>
#include <spdlog/spdlog.h>

#include "Scanner.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.Streams.h>

#include "WinRtUtils.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::Advertisement;

Scanner::Scanner(const std::unordered_map<UUID, GattService, UUID::Hash> &supportedGattServices): supportedGattServices(
    supportedGattServices) {
}

auto Scanner::startScan(const std::function<void(std::shared_ptr<Device>)> &receiver) -> void {
    resetPreviousScans();

    watcher->ScanningMode(BluetoothLEScanningMode::Active);

    watcher->Received(
        [receiver, this](BluetoothLEAdvertisementWatcher const &,
                         BluetoothLEAdvertisementReceivedEventArgs const &args) {
            auto name = args.Advertisement().LocalName();
            const uint64_t address = args.BluetoothAddress();

            const auto services = args.Advertisement().ServiceUuids();
            const auto services_size = services.Size();

            auto supported_services = std::unordered_set<GattService, GattService::Hash>();
            for (auto i = 0; i < services_size; i++) {
                auto cs = services.GetAt(i);

                if (auto candidate_service_uuid = WinrtUtils::uuidFromGuid(cs); supportedGattServices.contains(
                    candidate_service_uuid)) {
                    supported_services.insert(supportedGattServices.at(candidate_service_uuid));
                }
            }

            if (supported_services.empty()) return;

            if (name.empty()) {
                spdlog::info("No name found for device with address: {}", WinrtUtils::addressFromLong(address).value);
                const auto device = Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(address).
                        get();

                if (!device) {
                    spdlog::error("  Failed to connect to device.");
                    return;
                }

                spdlog::info("  Got device name: {}", WinrtUtils::nameFromHstring(name).value);
                name = device.Name();
            };

            const auto device = std::make_shared<Device>(
                WinrtUtils::nameFromHstring(name), WinrtUtils::addressFromLong(address), supported_services
            );

            receiver(device);
        });

    watcher->Start();
    spdlog::info("Scanning for BLE devices...");
}

auto Scanner::resetPreviousScans() -> void {
    this->stopScan();
    watcher = std::make_shared<BluetoothLEAdvertisementWatcher>();
}

auto Scanner::stopScan() const -> void {
    if (!watcher) return;

    spdlog::info("Stopping scan routine...");
    watcher->Stop();
}
