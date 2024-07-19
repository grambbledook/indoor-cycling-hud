#pragma once

#include "Data.h" // Assuming this includes Measurement and Device definitions
#include <memory>
#include <concepts>
#include <Events.h>

#include "DeviceRegistry.h"

template<DerivedFromMeasurement T>
class INotificationService {
public:
    explicit INotificationService(std::shared_ptr<DeviceRegistry> &registry, const Service &service);

    virtual ~INotificationService() = default;

    virtual void set_device(std::shared_ptr<Device> device);

    virtual void process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) = 0;

    virtual void process_measurement(std::shared_ptr<Device> device, const std::vector<uint8_t> &data) = 0;

public:
    Service service;
    std::shared_ptr<DeviceRegistry> &registry;
};

class HrmNotificationService final : public INotificationService<HrmMeasurement> {
public:
    // Constructor
    explicit HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry);

    // Destructor
    ~HrmNotificationService() override = default;

    // Override process_feature_and_set_devices
    void process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) override;

    // Override process_measurement
    void process_measurement(std::shared_ptr<Device> device, const std::vector<uint8_t> &data) override;

};

class CyclingCadenceAndSpeedNotificationService final : public INotificationService<HrmMeasurement> {
public:
    // Constructor
    explicit CyclingCadenceAndSpeedNotificationService(std::shared_ptr<DeviceRegistry> &registry);

    // Destructor
    ~CyclingCadenceAndSpeedNotificationService() override = default;

    // Override process_feature_and_set_devices
    void process_feature_and_set_devices(BleClient &client, std::shared_ptr<Device> device) override;

    // Override process_measurement
    void process_measurement(std::shared_ptr<Device> device, const std::vector<uint8_t> &data) override;

};