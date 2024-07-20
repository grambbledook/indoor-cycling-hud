#include "NotificationService.h"

#include <iostream>
#include <utility>

#include "Service.h"


template<DerivedFromMeasurement T>
INotificationService<
    T>::INotificationService(std::shared_ptr<DeviceRegistry> &registry, const Service &service): registry(registry),
    service(service) {
}

template<DerivedFromMeasurement T>
void INotificationService<T>::set_device(std::shared_ptr<Device> device) {
    std::cout << "INotificationService::set_device" << std::endl;

    const auto client = this->registry->connect(*device);

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


HrmNotificationService::HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry): INotificationService(
    registry, Services::HRM) {
}


void HrmNotificationService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) {
}

void HrmNotificationService::process_measurement(const std::shared_ptr<Device> device,
                                                 const std::vector<uint8_t> &data) {
    const auto flag = data[0];

    int hrm;
    if (flag & 0x01) {
        hrm = static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 8);
    } else {
        hrm = static_cast<int>(data[1]);
    }

    const HrmMeasurement measurement(hrm);
    std::cout << "HRM: " << measurement.hrm << std::endl;
}

CyclingCadenceAndSpeedNotificationService::CyclingCadenceAndSpeedNotificationService(
    std::shared_ptr<DeviceRegistry> &registry): INotificationService(registry, Services::CSC) {
}

void CyclingCadenceAndSpeedNotificationService::process_feature_and_set_devices(BleClient &client,
    std::shared_ptr<Device> device) {
}

void CyclingCadenceAndSpeedNotificationService::process_measurement(std::shared_ptr<Device> device,
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
        std::cout << "Speed: " << speed.cwr << ", " << speed.lwet << std::endl;
    }

    if (flag & 0b10) {
        const auto ccr = static_cast<uint16_t>(data[0 + offset]) |
                         static_cast<uint16_t>(data[1 + offset]) << 8;
        const auto lcet = static_cast<uint16_t>(data[2 + offset]) |
                          static_cast<uint16_t>(data[3 + offset]) << 8;

        const CadenceMeasurement cadence(ccr, lcet);
        std::cout << "Cadence: " << cadence.ccr << ", " << cadence.lcet << std::endl;
    }
}

PowerNotificationService::PowerNotificationService(std::shared_ptr<DeviceRegistry> &registry): INotificationService(
    registry, Services::PWR) {
}

void PowerNotificationService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) {
}

void PowerNotificationService::process_measurement(std::shared_ptr<Device> device, const std::vector<uint8_t> &data) {
    const auto value = static_cast<int>(data[2]) | (static_cast<int>(data[3]) << 8);

    const PowerMeasurement power(value);
    std::cout << "Power: " << power.power << std::endl;
}

FecService::FecService(std::shared_ptr<DeviceRegistry> &registry): INotificationService(registry, Services::LEGACY_BIKE_TRAINER){
}

void FecService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) {
}

void FecService::process_measurement(std::shared_ptr<Device> device, const std::vector<uint8_t> &data) {
    // int payload_size = data[1];
    // std::vector<uint8_t> message(data.begin() + 4, data.begin() + 4 + payload_size - 1);
    // int page_type = message[0];
    //
    // try {
    //     if (page_type == 16) {
    //         auto event = parse_general_data_page(device, message);
    //         model.update_general_data(event);
    //     } else if (page_type == 17) {
    //         auto event = parse_general_settings_page(device, message);
    //         model.update_general_settings(event);
    //     } else if (page_type == 25) {
    //         auto event = parse_specific_trainer_data_page(device, message);
    //         model.update_specific_trainer_data(event);
    //     } else {
    //         return;
    //     }
    // } catch (const std::exception& e) {
    //     std::cout << "Error processing measurement: " << e.what() << std::endl;
    // }
}

FeStateEvent FecService::parse_fe_state_event(int fe_state_bit) {
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
    return FeStateEvent(fe_state, lap_toggle);
}
