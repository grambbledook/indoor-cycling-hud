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
        std::cout << "  Connection result obtained." << std::endl;

        if (!connection_result) {
            std::cout << "  Failed to connect to device." << std::endl;
            return;
        }

        connection = std::make_shared<BluetoothLEDevice>(std::move(connection_result));
        std::cout << "  Device connected successfully." << std::endl;

        fetchCharacteristics();
        std::cout << "  Connection status: " << static_cast<int>(connection->ConnectionStatus()) << std::endl;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "  Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "  Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "  Unknown exception occurred." << std::endl;
    }
}

bool BleClient::subscribe(const UUID &characteristicUuid,
                          const std::function<void(std::shared_ptr<Device>, std::vector<uint8_t>)> &receiver) const {
    std::cout << "Subscribing to characteristic..." << characteristicUuid.value << std::endl;
    try {
        if (not characteristics.contains(characteristicUuid)) {
            std::cerr << "  Service or characteristic " << characteristicUuid.value << " not found." << std::endl;
            return false;
        }

        const auto anyChar = characteristics.at(characteristicUuid);
        const auto characteristic = std::any_cast<GattCharacteristic>(anyChar);
        characteristic.ValueChanged(
            [this, receiver](GattCharacteristic const &sender, GattValueChangedEventArgs const &args) {
                const auto reader = DataReader::FromBuffer(args.CharacteristicValue());
                std::vector<uint8_t> data(reader.UnconsumedBufferLength());
                reader.ReadBytes(data);

                receiver(std::make_shared<Device>(device), data);
            }
        );

        const auto status = characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
            GattClientCharacteristicConfigurationDescriptorValue::Notify
        ).get();

        if (status == GattCommunicationStatus::Success) {
            std::cout << "  Successfully subscribed to characteristic." << std::endl;
        } else {
            std::cout << "  Failed to subscribe to characteristic." << std::endl;
        }

        return status == GattCommunicationStatus::Success;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "  Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "  Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "  Unknown exception occurred." << std::endl;
    }
    return false;
}

bool BleClient::unsubscribe(const UUID &characteristicUuid) const {
    std::cout << "Unsubscribing from characteristic..." << characteristicUuid.value << std::endl;
    try {
        const auto anyChar = characteristics.at(characteristicUuid);
        if (not anyChar.has_value()) {
            std::cerr << "  Service or characteristic not found." << std::endl;
            return false;
        }

        const auto characteristic = std::any_cast<GattCharacteristic>(anyChar);
        const auto status = characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
            GattClientCharacteristicConfigurationDescriptorValue::None
        ).get();

        if (status == GattCommunicationStatus::Success) {
            std::cout << "  Successfully unsubscribed from characteristic." << std::endl;
        } else {
            std::cout << "  Failed to unsubscribe from characteristic." << std::endl;
        }

        return status == GattCommunicationStatus::Success;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "  Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "  Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "  Unknown exception occurred." << std::endl;
    }
    return false;
}

void BleClient::disconnect() const {
    if (isConnected()) {
        connection->Close();
    }
}

bool BleClient::isConnected() const {
    if (!connection) {
        std::cout << "  Connection is null." << std::endl;
        return false;
    }

    std::cout << "  Connection status: " << static_cast<int>(connection->ConnectionStatus()) << std::endl;

    return connection->ConnectionStatus() == BluetoothConnectionStatus::Connected;
}

std::pair<std::vector<uint8_t>, bool> BleClient::read(const UUID &characteristicUuid) const {
    std::cout << "Reading characteristic..." << characteristicUuid.value << std::endl;
    if (!isConnected()) {
        std::cout << "  Not connected to any device." << std::endl;
        return {{}, false};
    }

    if (not characteristics.contains(characteristicUuid)) {
        std::cout << "  Service or characteristic " << characteristicUuid.value << " not found." << std::endl;
        return {{}, false};
    }

    const auto anyChar = characteristics.at(characteristicUuid);
    const auto characteristic = std::any_cast<GattCharacteristic>(anyChar);

    const auto readResult = characteristic.ReadValueAsync().get();
    if (readResult.Status() != GattCommunicationStatus::Success) {
        return {{}, false};
    }

    const auto reader = DataReader::FromBuffer(readResult.Value());
    std::vector<uint8_t> data(reader.UnconsumedBufferLength());
    reader.ReadBytes(data);

    return {data, true};
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
        std::cerr << "  Done. Found " << characteristics.size() << " characteristics." << std::endl;
    } catch (const winrt::hresult_error &e) {
        std::cerr << "  Exception: " << winrt::to_string(e.message()) << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "  Standard exception: " << e.what() << std::endl;
        throw e;
    } catch (...) {
        std::cerr << "  Unknown exception occurred." << std::endl;
    }
}
