#include "NotificationService.h"

#include <iostream>

#include "Service.h"


template<DerivedFromMeasurement T>
INotificationService<T>::INotificationService(std::shared_ptr<DeviceRegistry> &registry): registry(registry) {
}

HrmNotificationService::HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry): INotificationService(
    registry) {
}

void HrmNotificationService::set_device(std::shared_ptr<Device> device) {
    std::cout << "HrmNotificationService::set_device" << std::endl;

    const auto client = registry->connect(*device);

    auto lambda = [this](const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) {
        this->process_measurement(device, data);
    };

    std::cout << "HrmNotificationService::set_device: Subscribing to HRM service" << std::endl;
    const auto result = client->subscribe(Services::HRM.characteristic_uuid, lambda);
    std::cout << "Subscribed to HRM service: " << result << std::endl;
}

void HrmNotificationService::unsubscribe() {
}

void HrmNotificationService::process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) {
}

void HrmNotificationService::process_measurement(const std::shared_ptr<Device> device, const std::vector<uint8_t> &data) {
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
