#pragma once
#include <mutex>

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
        std::cout << "Setting device dialog: " << deviceDialog << std::endl;
        this->deviceDialog = deviceDialog;
    }

    void deviceDiscovered(const DeviceDiscovered &data) const {
        std::cout << "QtEventPublisher::deviceDiscovered" << std::endl;
        auto event = new DeviceDiscoveredEvent(data);
        std::cout << "  Device discovered: " << data.device->name.value << std::endl;
        QCoreApplication::postEvent(deviceDialog.get(), event);
        std::cout << "  Event posted" << typeid(event).name() <<std::endl;
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
        std::cout << "QtEventPublisher::measurementReceived" << std::endl;

        const auto firstEvent = new MeasurementReceivedEvent(data);
        QCoreApplication::postEvent(sensorsWindow.get(), firstEvent);
        std::cout << "  Event one posted" << typeid(firstEvent).name() <<std::endl;

        const auto secondEvent = new MeasurementReceivedEvent(data);
        QCoreApplication::postEvent(workoutWindow.get(), secondEvent);
        std::cout << "  Event two posted" << typeid(secondEvent).name() <<std::endl;
    }

private:
    std::mutex mutex;
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
};
