#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <iostream>
#include <memory>
#include "BleClient.h"

#include <expected>

#include "WinRtUtils.h"
#include "spdlog/spdlog.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;

BleClient::BleClient(
    Device device,
    const std::function<void(BleClientEvent)> &eventReceiver
) : device(std::move(device)), eventReceiver(eventReceiver) {
}

auto BleClient::connect() -> void {
    try {
        const auto deviceAddress = WinrtUtils::addressToUint64T(device.address);

        spdlog::info("Attempting to connect to device with address: {}", device.address.value);
        auto connectionResult = BluetoothLEDevice::FromBluetoothAddressAsync(deviceAddress).get();

        spdlog::info("  Connection result obtained.");
        if (!connectionResult) {
            spdlog::info("  Failed to connect to device.");
            return;
        }

        connection = std::make_shared<BluetoothLEDevice>(std::move(connectionResult));
        connection->ConnectionStatusChanged(
            [this](BluetoothLEDevice const &sender, IInspectable const &args) {
                const auto status = sender.ConnectionStatus() == BluetoothConnectionStatus::Connected
                                        ? BleConnectionStatus::CONNECTED
                                        : BleConnectionStatus::DISCONNECTED;

                eventReceiver(BleClientEvent(std::make_shared<Device>(device), status));
            }
        );


        spdlog::info("  Device connected successfully.");

        fetchCharacteristics();
        spdlog::info("  Connection status: {}", static_cast<int>(connection->ConnectionStatus()));
    } catch (const hresult_error &e) {
        spdlog::error("  Exception: {}", winrt::to_string(e.message()));
    } catch (const std::exception &e) {
        spdlog::error("  Standard exception: {}", e.what());
    } catch (...) {
        spdlog::error("  Unknown exception occurred.");
    }
}

auto BleClient::subscribe(const UUID &characteristicUuid,
                          const std::function<void(std::shared_ptr<Device>, std::vector<unsigned char>)> &receiver)
const -> bool {
    spdlog::info("Subscribing to characteristic: {}", characteristicUuid.value);
    try {
        if (not characteristics.contains(characteristicUuid)) {
            spdlog::info("  Service or characteristic {} not found.", characteristicUuid.value);
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
            spdlog::info("  Successfully subscribed to characteristic.");
        } else {
            spdlog::info("  Failed to subscribe to characteristic.");
        }

        return status == GattCommunicationStatus::Success;
    } catch (const winrt::hresult_error &e) {
        spdlog::error("  Exception: {}", winrt::to_string(e.message()));
    }
    return false;
}

auto BleClient::unsubscribe(const UUID &characteristicUuid) const -> bool {
    spdlog::info("Unsubscribing from characteristic: {}", characteristicUuid.value);

    try {
        const auto uuid = characteristics.at(characteristicUuid);

        if (not uuid.has_value()) {
            spdlog::info("  Service or characteristic {} not found.", characteristicUuid.value);
            return false;
        }

        const auto characteristic = std::any_cast<GattCharacteristic>(uuid);
        const auto status = characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
            GattClientCharacteristicConfigurationDescriptorValue::None
        ).get();

        if (status == GattCommunicationStatus::Success) {
            spdlog::info("  Successfully unsubscribed from characteristic.");
        } else {
            spdlog::info("  Failed to unsubscribe from characteristic.");
        }

        return status == GattCommunicationStatus::Success;
    } catch (const winrt::hresult_error &e) {
        spdlog::error("  Exception: {}", winrt::to_string(e.message()));
    }

    return false;
}

auto BleClient::disconnect() const -> void {
    if (!isConnected()) {
        return;
    }
    connection->Close();
}

auto BleClient::isConnected() const -> bool {
    if (!connection) {
        spdlog::info("  Connection is null.");
        return false;
    }

    spdlog::info("  Connection status: {}", static_cast<int>(connection->ConnectionStatus()));
    return connection->ConnectionStatus() == BluetoothConnectionStatus::Connected;
}

auto BleClient::read(const UUID &characteristicUuid) const -> std::expected<std::vector<unsigned char>, void *> {
    spdlog::info("Reading characteristic: {}", characteristicUuid.value);
    if (!isConnected()) {
        spdlog::info("  Not connected to any device.");
        return std::unexpected(nullptr);
    }

    if (!characteristics.contains(characteristicUuid)) {
        spdlog::info("  Service or characteristic {} not found.", characteristicUuid.value);
        return std::unexpected(nullptr);
    }

    const auto anyChar = characteristics.at(characteristicUuid);
    const auto characteristic = std::any_cast<GattCharacteristic>(anyChar);

    const auto readResult = characteristic.ReadValueAsync().get();
    if (readResult.Status() != GattCommunicationStatus::Success) {
        return std::unexpected(nullptr);
    }

    const auto reader = DataReader::FromBuffer(readResult.Value());
    std::vector<uint8_t> data(reader.UnconsumedBufferLength());
    reader.ReadBytes(data);

    return data;
}

auto BleClient::fetchCharacteristics() -> void {
    spdlog::debug("Fetching characteristics...");
    try {
        const auto services = connection->GetGattServicesAsync().get();

        for (const auto &gattService: services.Services()) {
            const auto [serviceId] = WinrtUtils::uuidFromGuid(gattService.Uuid());

            spdlog::debug("  Service UUID: {}", serviceId);
            auto characteristics = gattService.GetCharacteristicsAsync().get();
            for (const auto &gattCharacteristic: characteristics.Characteristics()) {
                const auto characteristic = WinrtUtils::uuidFromGuid(gattCharacteristic.Uuid());

                spdlog::debug("    Characteristic UUID: {}", characteristic.value);
                this->characteristics[characteristic] = gattCharacteristic;
            }
        }
        spdlog::debug("  Characteristics fetched.");
    } catch (const hresult_error &e) {
        spdlog::error("  Exception: {}", to_string(e.message()));
    }
}
