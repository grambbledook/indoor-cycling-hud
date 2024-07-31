#pragma once

#include <TrainerWindow.h>
#include <SensorsWindow.h>
#include <WorkoutWindow.h>

#include "Controller.h"
#include "DeviceDialog.h"


class ViewNavigator final : public QObject {
    Q_OBJECT

public:
    explicit ViewNavigator(
        const std::shared_ptr<ControllerHandler> &controllerHandler,
        const std::shared_ptr<ShowDeviceDialogController> &deviceDialogController,
        const std::shared_ptr<ConnectToDeviceController> &connectToDeviceController,
        const std::shared_ptr<TrainerWindowController> &trainerWindowController,
        const std::shared_ptr<SensorsWindowController> &sensorsWindowController,
        const std::shared_ptr<WorkoutWindowController> &workoutWindowController,
        const std::shared_ptr<ShutdownController> &shutdownController
    );

public slots:
    void nextScreen(const std::string &command) const;

private:
    std::shared_ptr<ControllerHandler> controllerHandler;
    std::shared_ptr<ConnectToDeviceController> connectToDeviceController;
    std::shared_ptr<ShowDeviceDialogController> deviceDialogController;
    std::shared_ptr<TrainerWindowController> trainerWindowController;
    std::shared_ptr<SensorsWindowController> sensorsWindowController;
    std::shared_ptr<WorkoutWindowController> workoutWindowController;
    std::shared_ptr<ShutdownController> shutdownController;

    int x;
    int y;
};
