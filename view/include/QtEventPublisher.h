#pragma once
#include <mutex>
#include <spdlog/spdlog.h>

#include "DeviceDialog.h"
#include "Events.h"
#include "SystemTray.h"
#include "DeviceWindow.h"
#include "WorkoutSummaryWindow.h"
#include "WorkoutWindow.h"

class QtEventPublisher final {
public:
    explicit QtEventPublisher(
        const std::shared_ptr<DeviceWindow> &deviceWindow,
        const std::shared_ptr<WorkoutWindow> &workoutWindow,
        const std::shared_ptr<WorkoutSummaryWindow> &workoutSummaryWindow,
        const std::shared_ptr<SystemTray> &tray
    ): deviceWindow(deviceWindow), workoutWindow(workoutWindow), workoutSummaryWindow(workoutSummaryWindow), tray(tray) {
    }

    auto setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) -> void {
        std::lock_guard guard(mutex);
        this->deviceDialog = deviceDialog;
    }

    auto deviceDiscovered(const DeviceDiscovered &data) const -> void {
        const auto first = new DeviceDiscoveredEvent(data);
        QCoreApplication::postEvent(deviceDialog.get(), first);

        const auto second = new DeviceDiscoveredEvent(data);
        QCoreApplication::postEvent(tray.get(), second);
    }

    auto deviceSelected(const DeviceSelected &data) const -> void {
        const auto firstEvent = new DeviceSelectedEvent(data);
        QCoreApplication::postEvent(deviceWindow.get(), firstEvent);
    }

    auto workoutData(const WorkoutEvent &data) const -> void {
        const auto firstEvent = new WorkoutEventEvent(data);
        QCoreApplication::postEvent(workoutWindow.get(), firstEvent);
    }

    auto workoutSummary(const WorkoutEvent &data) const -> void {
        const auto event = new WorkoutSummaryEvent(data);
        QCoreApplication::postEvent(workoutSummaryWindow.get(), event);
    }

private:
    std::mutex mutex;
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<DeviceWindow> deviceWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
    std::shared_ptr<WorkoutSummaryWindow> workoutSummaryWindow;
    std::shared_ptr<SystemTray> tray;
};
