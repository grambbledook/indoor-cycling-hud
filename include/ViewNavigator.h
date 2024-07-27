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
        const std::shared_ptr<DeviceDialogController> &deviceDialogController,
        const std::shared_ptr<TrainerWindowController> &trainerWindowController,
        const std::shared_ptr<SensorsWindowController> &sensorsWindowController,
        const std::shared_ptr<WorkoutWindowController> &workoutWindowController
    );

public slots:
    void nextScreen(const std::string &screen) const;

private:
    std::shared_ptr<ControllerHandler> controllerHandler;

    std::shared_ptr<DeviceDialogController> deviceDialogController;
    std::shared_ptr<TrainerWindowController> trainerWindowController;
    std::shared_ptr<SensorsWindowController> sensorsWindowController;
    std::shared_ptr<WorkoutWindowController> workoutWindowController;

    int x;
    int y;
};
