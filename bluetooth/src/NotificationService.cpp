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

    std::cout << "INotificationService::set_device: Subscribing to " << service.type << "service" << std::endl;
    const auto result = client->subscribe(Services::HRM.characteristic_uuid, lambda);
    std::cout << "Subscribed to " << service.type << " service: " << result << std::endl;
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
