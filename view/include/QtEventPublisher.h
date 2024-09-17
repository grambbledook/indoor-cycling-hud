#pragma once
#include <mutex>
#include <spdlog/spdlog.h>

#include "DeviceDialog.h"
#include "Events.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutSummaryWindow.h"
#include "WorkoutWindow.h"

class QtEventPublisher final {
public:
    explicit QtEventPublisher(
        const std::shared_ptr<TrainerWindow> &trainerWindow,
        const std::shared_ptr<SensorsWindow> &sensorsWindow,
        const std::shared_ptr<WorkoutWindow> &workoutWindow,
        const std::shared_ptr<WorkoutSummaryWindow> &workoutSummaryWindow
    ): trainerWindow(trainerWindow), sensorsWindow(sensorsWindow),
       workoutWindow(workoutWindow), workoutSummaryWindow(workoutSummaryWindow) {
    }

    void setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) {
        std::lock_guard guard(mutex);
        this->deviceDialog = deviceDialog;
    }

    void deviceDiscovered(const DeviceDiscovered &data) const {
        const auto event = new DeviceDiscoveredEvent(data);
        QCoreApplication::postEvent(deviceDialog.get(), event);
    }

    void deviceSelected(const DeviceSelected &data) const {
        auto a = std::make_shared<DeviceSelected>(data);
        auto b = std::make_shared<DeviceSelected>(data);

        const auto firstEvent = new DeviceSelectedEvent(data);
        QCoreApplication::postEvent(trainerWindow.get(), firstEvent);

        const auto secondEvent = new DeviceSelectedEvent(data);
        QCoreApplication::postEvent(sensorsWindow.get(), secondEvent);
    }

    void workoutData(const WorkoutEvent &data) const {
        const auto firstEvent = new WorkoutEventEvent(data);
        QCoreApplication::postEvent(sensorsWindow.get(), firstEvent);

        const auto secondEvent = new WorkoutEventEvent(data);
        QCoreApplication::postEvent(workoutWindow.get(), secondEvent);
    }

    void workoutSummary(const WorkoutEvent &data) const {
        const auto event = new WorkoutSummaryEvent(data);
        QCoreApplication::postEvent(workoutSummaryWindow.get(), event);
    }

private:
    std::mutex mutex;
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
    std::shared_ptr<WorkoutSummaryWindow> workoutSummaryWindow;
};
