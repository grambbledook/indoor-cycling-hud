#pragma once

#include <QEvent>
#include <memory>
#include "Data.h"
#include "Stats.h"
const auto DeviceDiscoveredType = static_cast<QEvent::Type>(QEvent::registerEventType());
const auto DeviceSelectedType = static_cast<QEvent::Type>(QEvent::registerEventType());

// this code poses danger as it is inlined and static variable might be creted in multiple translation units
inline QEvent::Type getDeviceDiscoveredType() {
    static const auto DeviceDiscoveredType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return DeviceDiscoveredType;
}

inline QEvent::Type getDeviceSelectedType() {
    static const auto DeviceSelectedType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return DeviceSelectedType;
}

inline QEvent::Type getMeasurementReceivedType() {
    static const auto MeasurementReceivedType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return MeasurementReceivedType;
}

class DeviceDiscoveredEvent final : public QEvent {
public:
    explicit DeviceDiscoveredEvent(std::shared_ptr<Device> device)
        : QEvent(getDeviceDiscoveredType()), device(std::move(device)) {
    }

    [[nodiscard]] std::shared_ptr<Device> getDevice() const { return device; }

private:
    std::shared_ptr<Device> device;
};

class DeviceSelectedEvent final : public QEvent {
public:
    explicit DeviceSelectedEvent(std::shared_ptr<Device> device)
        : QEvent(getDeviceSelectedType()), device(std::move(device)) {
    }

    [[nodiscard]] std::shared_ptr<Device> getDevice() const { return device; }

private:
    std::shared_ptr<Device> device;
};

class MeasurementReceivedEvent final : public QEvent {
public:
    explicit MeasurementReceivedEvent(MeasurementsUpdate data)
        : QEvent(getMeasurementReceivedType()), data(data) {
    }

    [[nodiscard]] MeasurementsUpdate getData() const { return data; }

private:
    MeasurementsUpdate data;
};
