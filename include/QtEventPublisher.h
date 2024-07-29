#pragma once
#include <mutex>

#include "DeviceDialog.h"
#include "Events.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"

class QtEventPublisher final {
public:
    explicit QtEventPublisher(
        const std::shared_ptr<TrainerWindow> &trainerWindow,
        const std::shared_ptr<SensorsWindow> &sensorsWindow
    ): trainerWindow(trainerWindow), sensorsWindow(sensorsWindow) {
    }

    void setDeviceDialog(const std::shared_ptr<DeviceDialog> &deviceDialog) {
        std::lock_guard guard(mutex);
        std::cout << "Setting device dialog: " << deviceDialog << std::endl;
        this->deviceDialog = deviceDialog;
        if (this->deviceDialog != nullptr) {
            std::cout << " Device is set: " << std::endl;
        }
    }

    void deviceDiscovered(const std::shared_ptr<Device> &device) const {
        std::cout << "QtAdapter::deviceDiscovered" << std::endl;
        auto event = new DeviceDiscoveredEvent(device);
        std::cout << "  Device discovered: " << device->name.value << std::endl;
        QCoreApplication::postEvent(deviceDialog.get(), event);
        std::cout << "  Event posted" << typeid(event).name() <<std::endl;
    }

    void deviceSelected(const std::shared_ptr<Device> &device) const {
        const auto firstEvent = new DeviceSelectedEvent(device);
        QCoreApplication::postEvent(trainerWindow.get(), firstEvent);

        const auto secondEvent = new DeviceSelectedEvent(device);
        QCoreApplication::postEvent(sensorsWindow.get(), secondEvent);
    }

private:
    std::shared_ptr<DeviceDialog> deviceDialog;
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::mutex mutex;
};
