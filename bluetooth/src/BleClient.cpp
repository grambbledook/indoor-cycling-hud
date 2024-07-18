#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <iostream>
#include <memory>
#include "BleClient.h"
#include "WinRtUtils.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;

BleClient::BleClient(Device device) : device(std::move(device)) {
}

void BleClient::connect() {
    try {
        const auto device_address = WinrtUtils::address_to_uint64_t(device.address);

        std::cout << "Attempting to connect to device with address: " << device.address.value << std::endl;
        auto connection_result = BluetoothLEDevice::FromBluetoothAddressAsync(device_address).get();
        std::cout << "Connection result obtained." << std::endl;

        if (!connection_result) {
            std::cerr << "Failed to connect to device." << std::endl;
            return;
        }

        connection = std::make_shared<BluetoothLEDevice>(std::move(connection_result));
        std::cout << "Device connected successfully." << std::endl;

        fetchCharacteristics();
        std::cout << "Connection status: " << static_cast<int>(connection->ConnectionStatus()) << std::endl;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }
}

bool BleClient::subscribe(const UUID &characteristicUuid,
                          const std::function<void(std::vector<uint8_t>)> &receiver) const {
    try {
        const auto anyChar = characteristics.at(characteristicUuid);
        if (not anyChar.has_value()) {
            std::cerr << "Service or characteristic not found." << std::endl;
            return false;
        }

        const auto characteristic = std::any_cast<GattCharacteristic>(anyChar);
        characteristic.ValueChanged(
            [receiver](GattCharacteristic const &sender, GattValueChangedEventArgs const &args) {
                const auto reader = DataReader::FromBuffer(args.CharacteristicValue());
                std::vector<uint8_t> data(reader.UnconsumedBufferLength());
                std::cout << "   a";
                reader.ReadBytes(data);
                std::cout << "   b";
                receiver(data);
                std::cout << "   c";
            }
        );

        const auto status = characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
            GattClientCharacteristicConfigurationDescriptorValue::Notify
        ).get();

        if (status == GattCommunicationStatus::Success) {
            std::cout << "Successfully subscribed to characteristic." << std::endl;
        } else {
            std::cerr << "Failed to subscribe to characteristic." << std::endl;
        }

        return status == GattCommunicationStatus::Success;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }
    return false;
}

void BleClient::disconnect() const {
    if (isConnected()) {
        connection->Close();
    }

    // uninit_apartment();
}

bool BleClient::isConnected() const {
    if (!connection) {
        std::cout << "Connection is null." << std::endl;
        return false;
    }

    std::cout << "Connection status: " << static_cast<int>(connection->ConnectionStatus()) << std::endl;

    return connection->ConnectionStatus() == BluetoothConnectionStatus::Connected;
}

void BleClient::read(UUID serviceUuid, UUID characteristicUuid) {
}

void BleClient::fetchCharacteristics() {
    std::cout << "Fetching characteristics..." << std::endl;
    try {
        const auto services = connection->GetGattServicesAsync().get();

        for (const auto &service: services.Services()) {
            auto serviceUuid = WinrtUtils::uuid_from_guid(service.Uuid());
            std::cout << "  Service UUID: " << serviceUuid.value << std::endl;

            auto characteristics = service.GetCharacteristicsAsync().get();

            for (const auto &characteristic: characteristics.Characteristics()) {
                auto charUuid = WinrtUtils::uuid_from_guid(characteristic.Uuid());
                std::cout << "    Characteristic UUID: " << charUuid.value << std::endl;

                this->characteristics[charUuid] = characteristic;
            }
        }
        std::cerr << "Done. Found " << characteristics.size() << " characteristics." << std::endl;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        throw e;
    } catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }
}
