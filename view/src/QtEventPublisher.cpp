#pragma once
#include <mutex>

#include "QtEventPublisher.h"

QtEventPublisher::QtEventPublisher(
    const std::shared_ptr<DeviceWindow> &deviceWindow,
    const std::shared_ptr<WorkoutWindow> &workoutWindow,
    const std::shared_ptr<WorkoutSummaryWindow> &workoutSummaryWindow,
    const std::shared_ptr<SystemTray> &tray
): deviceWindow(deviceWindow), workoutWindow(workoutWindow), workoutSummaryWindow(workoutSummaryWindow), tray(tray) {
}

auto QtEventPublisher::setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) -> void {
    std::lock_guard guard(mutex);
    this->deviceDialog = deviceDialog;
}

auto QtEventPublisher::deviceDiscovered(const DeviceDiscovered &data) const -> void {
    const auto first = new DeviceDiscoveredEvent(data);
    QCoreApplication::postEvent(deviceDialog.get(), first);

    const auto second = new DeviceDiscoveredEvent(data);
    QCoreApplication::postEvent(tray.get(), second);
}

auto QtEventPublisher::deviceSelected(const DeviceSelected &data) const -> void {
    const auto firstEvent = new DeviceSelectedEvent(data);
    QCoreApplication::postEvent(deviceWindow.get(), firstEvent);
}

auto QtEventPublisher::workoutData(const WorkoutEvent &data) const -> void {
    const auto firstEvent = new WorkoutEventEvent(data);
    QCoreApplication::postEvent(workoutWindow.get(), firstEvent);
}

auto QtEventPublisher::workoutSummary(const WorkoutEvent &data) const -> void {
    const auto event = new WorkoutSummaryEvent(data);
    QCoreApplication::postEvent(workoutSummaryWindow.get(), event);
}
