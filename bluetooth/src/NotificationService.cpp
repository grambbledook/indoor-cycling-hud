#include "NotificationService.h"

#include <iostream>
#include <utility>

#include "BluetoothConstants.h"
#include "Service.h"


template<DerivedFromMeasurement T>
INotificationService<
    T>::INotificationService(std::shared_ptr<DeviceRegistry> &registry,
                             std::shared_ptr<Model> &model,
                             const GattService &service): registry(registry),
                                                          service(service),
                                                          model(model) {
}

template<DerivedFromMeasurement T>
void INotificationService<T>::setDevice(std::shared_ptr<Device> device) {
    std::cout << "INotificationService::set_device" << std::endl;

    const auto client = this->registry->connect(*device);

    this->processFeatureAndSetDevices(*client, device);

    auto lambda = [this](const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
        this->processMeasurement(device, data);
    };

    std::cout << "  INotificationService::set_device: Subscribing to " << service.type << " service" << std::endl;
    const auto result = client->subscribe(service.characteristic_uuid, lambda);
    std::cout << "  Subscribed to " << service.type << " service: " << result << std::endl;
}

template<DerivedFromMeasurement T>
void INotificationService<T>::unsetDevice(std::shared_ptr<Device> device) {
    std::cout << "INotificationService::unset_device" << std::endl;

    const auto client = this->registry->connect(*device);

    std::cout << "  INotificationService::unset_device: Unsubscribing from " << service.type << " service" << std::endl;
    const auto result = client->unsubscribe(service.characteristic_uuid);
    std::cout << "  Unsubscribed from " << service.type << " service: " << result << std::endl;
}


HrmNotificationService::HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                               std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::HRM) {
}


void HrmNotificationService::processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) {
    model->setHeartRateMonitor(device);
}

void HrmNotificationService::processMeasurement(const std::shared_ptr<Device> &device,
                                                 const std::vector<uint8_t> &data) {
    const auto flag = data[0];

    int hrm;
    if (flag & 0x01) {
        hrm = static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 8);
    } else {
        hrm = static_cast<int>(data[1]);
    }

    const HrmMeasurement measurement(hrm);
    model->recordHeartData(MeasurementEvent(device, measurement));
}

CyclingCadenceAndSpeedNotificationService::CyclingCadenceAndSpeedNotificationService(
    std::shared_ptr<DeviceRegistry> &registry, std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::CSC) {
}

void CyclingCadenceAndSpeedNotificationService::processFeatureAndSetDevices(BleClient &client,
    std::shared_ptr<Device> &device) {
    auto [data, success] = client.read(UUID("00002a5c-0000-1000-8000-00805f9b34fb"));

    if (not success) {
        std::cerr << "Failed to read CSC feature." << std::endl;
        return;
    }

    const auto flag = data[0];
    if (flag & 0b01) {
        model->setSpeedSensor(device);
    }

    if (flag & 0b10) {
        model->setCadenceSensor(device);
    }
}

void CyclingCadenceAndSpeedNotificationService::processMeasurement(const std::shared_ptr<Device> &device,
                                                                    const std::vector<uint8_t> &data) {
    const auto flag = data[0];
    auto offset = 1;

    if (flag & 0b01) {
        const auto cwr = static_cast<uint32_t>(data[0 + offset]) |
                         static_cast<uint32_t>(data[1 + offset]) << 8 |
                         static_cast<uint32_t>(data[2 + offset]) << 16 |
                         static_cast<uint32_t>(data[3 + offset]) << 24;

        const auto lwet = static_cast<uint16_t>(data[4 + offset]) |
                          static_cast<uint16_t>(data[5 + offset]) << 8;
        offset += 6;

        const SpeedMeasurement speed(cwr, lwet);
        model->recordSpeedData(MeasurementEvent(device, speed));
    }

    if (flag & 0b10) {
        const auto ccr = static_cast<uint16_t>(data[0 + offset]) |
                         static_cast<uint16_t>(data[1 + offset]) << 8;
        const auto lcet = static_cast<uint16_t>(data[2 + offset]) |
                          static_cast<uint16_t>(data[3 + offset]) << 8;

        const CadenceMeasurement cadence(ccr, lcet);
        model->recordCadenceData(MeasurementEvent(device, cadence));
    }
}

PowerNotificationService::PowerNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                                   std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::PWR) {
}

void PowerNotificationService::processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) {
    model->setPowerMeter(device);
}

void PowerNotificationService::processMeasurement(const std::shared_ptr<Device> &device,
                                                   const std::vector<uint8_t> &data) {
    const auto value = static_cast<int>(data[2]) | (static_cast<int>(data[3]) << 8);

    const PowerMeasurement power(value);
    model->recordPowerData(MeasurementEvent(device, power));
}

FecService::FecService(std::shared_ptr<DeviceRegistry> &registry, std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::FEC_BIKE_TRAINER) {
}

void FecService::processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) {
    model->setBikeTrainer(device);
}

void FecService::processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
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

            const GeneralData event(elapsedTime, distanceTraveled, speed, heartRate, parseFeStateByte(message[7]));
            model->recordTrainerData(MeasurementEvent(device, event));
        } else if (pageType == 0x11) {
            const auto cycleLength = message[3];

            const auto inclineValue = message[4] << 8 | message[5];
            auto incline = inclineValue == 0x7FFF or inclineValue > 1000 or inclineValue < -1000
                               ? 0
                               : inclineValue / 100.0;

            const auto resistance = std::round(message[6] * 0.5);

            const GeneralSettings event(cycleLength, incline, resistance, parseFeStateByte(message[7]));
            model->recordTrainerData(MeasurementEvent(device, event));
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

            SpecificTrainerData event(
                updateEventCount,
                instantaneousCadence,
                instantaneousPower,
                accumulatedPower,
                targetPowerLimits,
                trainerStatus,
                parseFeStateByte(message[7])
            );
            model->recordTrainerData(MeasurementEvent(device, event));
        } else {
            std::cout << "Unknown page type: " << pageType << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error processing measurement: " << e.what() << std::endl;
    }
}

FeState FecService::parseFeStateByte(const unsigned char fitnessEquipmentStateBit) {
    const bool lapToggle = fitnessEquipmentStateBit & 0x80;
    switch (fitnessEquipmentStateBit & 0x40) {
        case 1: return FeState(BLE::Tacx::Status::ASLEEP, lapToggle);
        case 2: return FeState(BLE::Tacx::Status::READY, lapToggle);
        case 3: return FeState(BLE::Tacx::Status::IN_USE, lapToggle);
        case 4: return FeState(BLE::Tacx::Status::FINISHED, lapToggle);
        default: return FeState(BLE::Tacx::Status::UNKNOWN, lapToggle);
    }
}
