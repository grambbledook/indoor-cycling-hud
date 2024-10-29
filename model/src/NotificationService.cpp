#include "NotificationService.h"

#include <memory>
#include <spdlog/spdlog.h>
#include <utility>
#include "BluetoothConstants.h"
#include "BleDeviceServices.h"

INotificationService::INotificationService(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model,
    const GattService &service
): service(service),
   model(model),
   registry(registry) {
}

auto INotificationService::setDevice(const std::shared_ptr<Device> &device) -> void {
    spdlog::info("INotificationService::set_device");

    const auto client = this->registry->connect(*device);

    this->processFeatureAndSetDevices(client.get(), device);

    auto lambda = [this](const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
        this->processMeasurement(device, data);
    };

    spdlog::info("  INotificationService::set_device: Subscribing to {} service", service.type);
    const auto result = client->subscribe(service.characteristic_uuid, lambda);
    spdlog::info("  Subscribed to {} service: {}", service.type, result);
}

auto INotificationService::unsetDevice(const std::shared_ptr<Device> &device) -> void {
    spdlog::info("INotificationService::unset_device");

    const auto client = this->registry->connect(*device);

    spdlog::info("  INotificationService::unset_device: Unsubscribing from {} service", service.type);
    const auto result = client->unsubscribe(service.characteristic_uuid);
    spdlog::info("  Unsubscribed from {} service: {}", service.type, result);
}

HrmNotificationService::HrmNotificationService(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model
): INotificationService(registry, model, BLE::Services::HRM) {
}

auto HrmNotificationService::processFeatureAndSetDevices(
    BleClient *client,
    const std::shared_ptr<Device> &device
) -> void {
    model->setDevice(Service::HEART_RATE, device);
}

auto HrmNotificationService::processMeasurement(
    const std::shared_ptr<Device> &device,
    const std::vector<uint8_t> &data
) -> void {
    const auto flag = data[0];

    int hrm = static_cast<int>(data[1]);
    if (flag & 0x01) {
        hrm |= static_cast<int>(data[2]) << 8;
    }

    model->recordData(MeasurementEvent(device, std::make_shared<HrmMeasurement>(hrm)));
}

CyclingCadenceAndSpeedNotificationService::CyclingCadenceAndSpeedNotificationService(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model
): INotificationService(registry, model, BLE::Services::CSC) {
}

auto CyclingCadenceAndSpeedNotificationService::processFeatureAndSetDevices(
    BleClient *client,
    const std::shared_ptr<Device> &device
) -> void {
    const auto result = client->read(UUID("00002a5c-0000-1000-8000-00805f9b34fb"));

    if (!result.has_value()) {
        spdlog::error("Failed to read CSC feature.");
        return;
    }

    const auto &data = result.value();
    const auto flag = data[0];
    if (flag & 0b01) {
        model->setDevice(Service::SPEED, device);
    }

    if (flag & 0b10) {
        model->setDevice(Service::CADENCE, device);
    }
}

auto CyclingCadenceAndSpeedNotificationService::processMeasurement(
    const std::shared_ptr<Device> &device,
    const std::vector<uint8_t> &data
) -> void {
    const auto flag = data[0];
    auto offset = 1;

    if (flag & 0b01) {
        const auto cwr = static_cast<unsigned long>(data[0 + offset]) |
                         static_cast<unsigned long>(data[1 + offset]) << 8 |
                         static_cast<unsigned long>(data[2 + offset]) << 16 |
                         static_cast<unsigned long>(data[3 + offset]) << 24;

        const auto lwet = static_cast<unsigned short>(data[4 + offset]) |
                          static_cast<unsigned short>(data[5 + offset]) << 8;
        offset += 6;

        model->recordData(MeasurementEvent(device, std::make_shared<SpeedMeasurement>(cwr, lwet)));
    }

    if (flag & 0b10) {
        const auto ccr = static_cast<unsigned short>(data[0 + offset]) |
                         static_cast<unsigned short>(data[1 + offset]) << 8;
        const auto lcet = static_cast<unsigned short>(data[2 + offset]) |
                          static_cast<unsigned short>(data[3 + offset]) << 8;

        model->recordData(MeasurementEvent(device, std::make_shared<CadenceMeasurement>(ccr, lcet)));
    }
}

PowerNotificationService::PowerNotificationService(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model
): INotificationService(registry, model, BLE::Services::PWR) {
}

auto PowerNotificationService::processFeatureAndSetDevices(
    BleClient *client,
    const std::shared_ptr<Device> &device
) -> void {
    model->setDevice(Service::POWER, device);
}

auto PowerNotificationService::processMeasurement(
    const std::shared_ptr<Device> &device,
    const std::vector<uint8_t> &data
) -> void {
    const auto value = static_cast<int>(data[2]) | (static_cast<int>(data[3]) << 8);

    model->recordData(MeasurementEvent(device, std::make_shared<PowerMeasurement>(value)));
}

FecService::FecService(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model
): INotificationService(registry, model, BLE::Services::FEC_BIKE_TRAINER) {
}

auto FecService::processFeatureAndSetDevices(
    BleClient *client,
    const std::shared_ptr<Device> &device
) -> void {
    model->setDevice(Service::BIKE_TRAINER, device);
}

auto FecService::processMeasurement(
    const std::shared_ptr<Device> &device,
    const std::vector<uint8_t> &data
) -> void {
    int payloadSize = data[1];
    const std::vector message(data.begin() + 4, data.begin() + 4 + payloadSize - 1);
    int pageType = message[0];

    try {
        if (pageType == 0x10) {
            const auto elapsedTime = std::round(message[2] * 0.25);
            const auto distanceTraveled = message[3];

            double speedVal = message[4] | message[5] << 8;
            double speed = speedVal == 0xFFFF ? 0 : std::round(speedVal * 0.001);
            auto heartRate = message[6] == 0xFF ? 0 : message[6];

            const auto event = std::make_shared<GeneralData>(
                static_cast<int>(elapsedTime), distanceTraveled,
                static_cast<int>(speed), heartRate, parseFeStateByte(message[7])
            );
            model->recordData(MeasurementEvent(device, event));
        } else if (pageType == 0x11) {
            const auto cycleLength = message[3];

            const auto inclineValue = message[4] << 8 | message[5];
            auto incline = inclineValue == 0x7FFF or inclineValue > 1000 or inclineValue < -1000
                               ? 0
                               : inclineValue / 100.0;

            const auto resistance = std::round(message[6] * 0.5);

            const auto event = std::make_shared<GeneralSettings>(
                cycleLength,
                static_cast<int>(incline),
                static_cast<int>(resistance),
                parseFeStateByte(message[7])
            );
            model->recordData(MeasurementEvent(device, event));
        } else if (pageType == 0x19) {
            auto updateEventCount = message[1];

            auto instantaneousCadence = message[2] == 0xFF ? 0 : message[2];
            auto accumulatedPower = message[3] | message[4] << 8;

            auto powerLsb = message[5];
            auto powerMsb = message[6] & 0xF;

            auto instantaneousPower = powerLsb | powerMsb << 8;
            if (instantaneousPower == 0xFFF) instantaneousPower = 0;

            const auto trainerStatusFlags = (message[6] >> 4) & 0xF;

            auto trainerStatus = TrainerStatus{
                static_cast<bool>(trainerStatusFlags & 0x1),
                static_cast<bool>(trainerStatusFlags & 0x2),
                static_cast<bool>(trainerStatusFlags & 0x4)
            };

            std::string targetPowerLimits;
            switch (message[7] & 0x7) {
                case 0: targetPowerLimits = BLE::Tacx::Power::IN_RANGE;
                    break;
                case 1: targetPowerLimits = BLE::Tacx::Power::TOO_LOW;
                    break;
                case 2: targetPowerLimits = BLE::Tacx::Power::TOO_HIGH;
                    break;
                default: targetPowerLimits = BLE::Tacx::Power::UNKNOWN;
                    break;
            }

            const auto event = std::make_shared<SpecificTrainerData>(
                updateEventCount,
                instantaneousCadence,
                instantaneousPower,
                accumulatedPower,
                targetPowerLimits,
                trainerStatus,
                parseFeStateByte(message[7])
            );
            model->recordData(MeasurementEvent(device, event));
        } else {
            spdlog::info("Unknown page type: {}", pageType);
        }
    } catch (const std::exception &e) {
        spdlog::error("Error processing measurement: {}", e.what());
    }
}

auto FecService::parseFeStateByte(const unsigned char fitnessEquipmentStateBit) -> FeState {
    const bool lapToggle = fitnessEquipmentStateBit & 0x80;
    switch (fitnessEquipmentStateBit & 0x40) {
        case 1: return FeState(BLE::Tacx::Status::ASLEEP, lapToggle);
        case 2: return FeState(BLE::Tacx::Status::READY, lapToggle);
        case 3: return FeState(BLE::Tacx::Status::IN_USE, lapToggle);
        case 4: return FeState(BLE::Tacx::Status::FINISHED, lapToggle);
        default: return FeState(BLE::Tacx::Status::UNKNOWN, lapToggle);
    }
}
