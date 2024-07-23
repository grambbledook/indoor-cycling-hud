#include "NotificationService.h"

#include <iostream>
#include <utility>


template<DerivedFromMeasurement T>
INotificationService<
    T>::INotificationService(std::shared_ptr<DeviceRegistry> &registry,
                             std::shared_ptr<Model> &model,
                             const GattService &service): registry(registry),
                                                          service(service),
                                                          model(model) {
}

template<DerivedFromMeasurement T>
void INotificationService<T>::set_device(std::shared_ptr<Device> device) {
    std::cout << "INotificationService::set_device" << std::endl;

    const auto client = this->registry->connect(*device);

    this->process_feature_and_set_devices(*client, device);

    auto lambda = [this](const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
        this->process_measurement(device, data);
    };

    std::cout << "INotificationService::set_device: Subscribing to " << service.type << " service" << std::endl;
    const auto result = client->subscribe(service.characteristic_uuid, lambda);
    std::cout << "Subscribed to " << service.type << " service: " << result << std::endl;
}

template<DerivedFromMeasurement T>
void INotificationService<T>::unset_device(std::shared_ptr<Device> device) {
    std::cout << "INotificationService::unset_device" << std::endl;

    const auto client = this->registry->connect(*device);


    std::cout << "INotificationService::unset_device: Unsubscribing from " << service.type << " service" << std::endl;
    const auto result = client->unsubscribe(service.characteristic_uuid);
    std::cout << "Unsubscribed from " << service.type << " service: " << result << std::endl;
}


HrmNotificationService::HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                               std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::HRM) {
}


void HrmNotificationService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> &device) {
    model->setHeartRateMonitor(device);
}

void HrmNotificationService::process_measurement(const std::shared_ptr<Device> &device,
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

void CyclingCadenceAndSpeedNotificationService::process_feature_and_set_devices(BleClient &client,
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

void CyclingCadenceAndSpeedNotificationService::process_measurement(const std::shared_ptr<Device> &device,
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

void PowerNotificationService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> &device) {
    model->setPowerMeter(device);
}

void PowerNotificationService::process_measurement(const std::shared_ptr<Device> &device,
                                                   const std::vector<uint8_t> &data) {
    const auto value = static_cast<int>(data[2]) | (static_cast<int>(data[3]) << 8);

    const PowerMeasurement power(value);
    model->recordPowerData(MeasurementEvent(device, power));
}

FecService::FecService(std::shared_ptr<DeviceRegistry> &registry, std::shared_ptr<Model> &model): INotificationService(
    registry, model, Services::LEGACY_BIKE_TRAINER) {
}

void FecService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> &device) {
}

void FecService::process_measurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
    int payload_size = data[1];
    const std::vector message(data.begin() + 1, data.begin() + 4 + payload_size - 1);
    int page_type = message[0];

    try {
        if (page_type == 16) {
            auto elapsedTime = message[2] * 0.25;
            auto distanceTraveled = message[3];

            double speed = message[4] | message[5] << 8;

            if (speed != 65535) {
                speed = 0.001 * speed;
            }

            auto heartRate = message[6];
            if (heartRate == 255) {
                heartRate = 0;
            }

            const auto feState = parse_fe_state_event(message[7]);

            const GeneralData event(elapsedTime, distanceTraveled, speed, heartRate, feState);
            std::cout << "General data: " << event.elapsed_time << " " << event.distance_traveled << " "
                    << event.speed << " " << event.heart_rate << " " << event.feState.feState << " "
                    << event.feState.lapToggle << std::endl;

            // model->recordGeneralData(MeasurementEvent(device, event));
        }
        // } else if (page_type == 17) {
        // auto event = parse_general_settings_page(device, message);
        // model.update_general_settings(event);
        // } else if (page_type == 25) {
        // auto event = parse_specific_trainer_data_page(device, message);
        // model.update_specific_trainer_data(event);
    } catch (const std::exception &e) {
        std::cout << "Error processing measurement: " << e.what() << std::endl;
    }
}

FeState FecService::parse_fe_state_event(int fe_state_bit) {
    const bool lap_toggle = fe_state_bit & 0x8;
    const int code = fe_state_bit & 0x7;

    std::string fe_state;
    switch (code) {
        case 1:
            fe_state = "ASLEEP";
            break;
        case 2:
            fe_state = "READY";
            break;
        case 3:
            fe_state = "IN_USE";
            break;
        case 4:
            fe_state = "FINISHED";
            break;
        default:
            fe_state = "UNKNOWN";
            break;
    }
    return FeState(fe_state, lap_toggle);
}
