#pragma once

#include <QEvent>
#include <memory>
#include "Data.h"
const auto DeviceDiscoveredType = static_cast<QEvent::Type>(QEvent::registerEventType());
const auto DeviceSelectedType = static_cast<QEvent::Type>(QEvent::registerEventType());

class DeviceDiscoveredEvent final : public QEvent {
public:
    explicit DeviceDiscoveredEvent(std::shared_ptr<Device> device)
        : QEvent(DeviceDiscoveredType), device(std::move(device)) {}

    std::shared_ptr<Device> getDevice() const { return device; }

private:
    std::shared_ptr<Device> device;
};
class DeviceSelectedEvent final : public QEvent {
public:
    explicit DeviceSelectedEvent(std::shared_ptr<Device> device)
        : QEvent(DeviceSelectedType), device(std::move(device)) {}

    std::shared_ptr<Device> getDevice() const { return device; }

private:
    std::shared_ptr<Device> device;
};