#pragma once

#include <memory>

#include <DeviceEvents.h>
#include "ClientRegistry.h"
#include "Model.h"

template<DerivedFromMeasurement T>
class INotificationService {
public:
    explicit INotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                  std::shared_ptr<Model> &model,
                                  const GattService &service);

    virtual ~INotificationService() = default;

public:
    virtual void setDevice(std::shared_ptr<Device> device);
    virtual void unsetDevice(std::shared_ptr<Device> device);
    virtual void processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) = 0;
    virtual void processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) = 0;

public:
    GattService service;
    std::shared_ptr<Model> &model;
    std::shared_ptr<DeviceRegistry> &registry;
};

class HrmNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit HrmNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                    std::shared_ptr<Model> &model);

    ~HrmNotificationService() override = default;

public:
    void processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) override;
    void processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) override;
};

class CyclingCadenceAndSpeedNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit CyclingCadenceAndSpeedNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                                       std::shared_ptr<Model> &model);

    ~CyclingCadenceAndSpeedNotificationService() override = default;

public:
    void processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) override;
    void processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) override;
};


class PowerNotificationService final : public INotificationService<HrmMeasurement> {
public:
    explicit PowerNotificationService(std::shared_ptr<DeviceRegistry> &registry,
                                      std::shared_ptr<Model> &model);

    ~PowerNotificationService() override = default;

public:
    void processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) override;
    void processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) override;
};

class FecService final : public INotificationService<FecMeasurement> {
public:
    explicit FecService(std::shared_ptr<DeviceRegistry> &registry,
                        std::shared_ptr<Model> &model);

    ~FecService() override = default;

public:
    void processFeatureAndSetDevices(BleClient &client, std::shared_ptr<Device> &device) override;
    void processMeasurement(const std::shared_ptr<Device> &device, const std::vector<uint8_t> &data) override;

private:
    static FeState parseFeStateByte(const unsigned char fitnessEquipmentStateBit);
};
