#pragma once
#include <mutex>
#include <spdlog/spdlog.h>

#include "DeviceDialog.h"
#include "Events.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"

class QtEventPublisher final {
public:
    explicit QtEventPublisher(
        const std::shared_ptr<TrainerWindow> &trainerWindow,
        const std::shared_ptr<SensorsWindow> &sensorsWindow,
        const std::shared_ptr<WorkoutWindow> &workoutWindow
    ): trainerWindow(trainerWindow), sensorsWindow(sensorsWindow), workoutWindow(workoutWindow) {
    }

    void setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) {
        std::lock_guard guard(mutex);
        this->deviceDialog = deviceDialog;
    }

    void deviceDiscovered(const DeviceDiscovered &data) const {
        auto event = new DeviceDiscoveredEvent(data);
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

    void measurementReceived(const WorkoutData &data) const {
        const auto firstEvent = new MeasurementReceivedEvent(data);
        QCoreApplication::postEvent(sensorsWindow.get(), firstEvent);

        const auto secondEvent = new MeasurementReceivedEvent(data);
        QCoreApplication::postEvent(workoutWindow.get(), secondEvent);
    }

private:
    std::mutex mutex;
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
};