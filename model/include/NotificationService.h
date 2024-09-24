#pragma once

#include <memory>

#include <BleDeviceEvents.h>
#include "ClientRegistry.h"
#include "Model.h"

template<DerivedFromMeasurement T>
class INotificationService {
public:
    explicit INotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                  std::shared_ptr<Model> &model,
                                  const GattService &service);

    virtual ~INotificationService() = default;

    virtual auto setDevice(const std::shared_ptr<Device> &device) -> void;

    virtual auto unsetDevice(const std::shared_ptr<Device> &device) -> void;

    virtual auto processFeatureAndSetDevices(BleClient *client, const std::shared_ptr<Device> &device) -> void = 0;

    virtual auto processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) -> void =
    0;

    GattService service;
    std::shared_ptr<Model> &model;
    std::shared_ptr<DeviceRegistry> &registry;
};

class HrmNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                    std::shared_ptr<Model> &model);

    ~HrmNotificationService() override = default;

    auto processFeatureAndSetDevices(BleClient *client, const std::shared_ptr<Device> &device) -> void override;

    auto processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) -> void override;
};

class CyclingCadenceAndSpeedNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit CyclingCadenceAndSpeedNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                                       std::shared_ptr<Model> &model);

    ~CyclingCadenceAndSpeedNotificationService() override = default;

    auto processFeatureAndSetDevices(BleClient *client, const std::shared_ptr<Device> &device) -> void override;

    auto processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) -> void override;
};


class PowerNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit PowerNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                      std::shared_ptr<Model> &model);

    ~PowerNotificationService() override = default;

    auto processFeatureAndSetDevices(BleClient *client, const std::shared_ptr<Device> &device) -> void override;

    auto processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) -> void override;
};

class FecService final : public INotificationService<FecMeasurement> {
public:
    explicit FecService(std::shared_ptr<DeviceRegistry> &registry,
                        std::shared_ptr<Model> &model);

    ~FecService() override = default;

    auto processFeatureAndSetDevices(BleClient *client, const std::shared_ptr<Device> &device) -> void override;

    auto processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) -> void override;

private:
    static FeState parseFeStateByte(const unsigned char fitnessEquipmentStateBit);
};
