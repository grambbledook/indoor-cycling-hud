#pragma once
#include <mutex>

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
    );

    auto setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) -> void;

    auto deviceDiscovered(const DeviceDiscovered &data) const -> void;

    auto deviceSelected(const DeviceSelected &data) const -> void;

    auto subscribedToService(const SubscribedToService &data) const -> void;

    auto workoutData(const WorkoutEvent &data) const -> void;

    auto workoutSummary(const WorkoutEvent &data) const -> void;

private:
    std::mutex mutex;
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<DeviceWindow> deviceWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
    std::shared_ptr<WorkoutSummaryWindow> workoutSummaryWindow;
    std::shared_ptr<SystemTray> tray;
};
