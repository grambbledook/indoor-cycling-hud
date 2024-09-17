#pragma once

#include "Controller.h"
#include "DeviceDialog.h"
#include <vector>
#include <any>

class ViewNavigator final : public QObject {
    Q_OBJECT

public:
    explicit ViewNavigator(
        const std::shared_ptr<ControllerHandler> &controllerHandler,
        const std::shared_ptr<ShowDeviceDialogController> &deviceDialogController,
        const std::shared_ptr<ConnectToDeviceController> &connectToDeviceController,
        const std::shared_ptr<TrainerWindowController> &trainerWindowController,
        const std::shared_ptr<SensorsWindowController> &sensorsWindowController,
        const std::shared_ptr<SelectWorkoutWindowController> &selectWorkoutWindowController,
        const std::shared_ptr<WorkoutWindowController> &workoutWindowController,
        const std::shared_ptr<WorkoutSummaryWindowController> &workoutSummaryWindowController,
        const std::shared_ptr<SwitchThemeController> &switchThemeController,
        const std::shared_ptr<ShutdownController> &shutdownController,
        const std::shared_ptr<WheelSizeSelectionController> &wheelSizeSelectionController,
        const std::shared_ptr<SpeedUnitController> &speedUnitController,
        const std::shared_ptr<TrayConnectToDeviceController> &trayConnectToDeviceController
    );

public slots:
    void nextScreen(const std::string &command, const std::vector<std::any> &args = {}) const;

private:
    std::shared_ptr<ControllerHandler> controllerHandler;
    std::shared_ptr<ConnectToDeviceController> connectToDeviceController;
    std::shared_ptr<ShowDeviceDialogController> deviceDialogController;
    std::shared_ptr<TrainerWindowController> trainerWindowController;
    std::shared_ptr<SensorsWindowController> sensorsWindowController;
    std::shared_ptr<SelectWorkoutWindowController> selectWorkoutWindowController;
    std::shared_ptr<WorkoutWindowController> workoutWindowController;
    std::shared_ptr<WorkoutSummaryWindowController> workoutSummaryWindowController;
    std::shared_ptr<SwitchThemeController> switchThemeController;
    std::shared_ptr<ShutdownController> shutdownController;
    std::shared_ptr<WheelSizeSelectionController> wheelSizeSelectionController;
    std::shared_ptr<SpeedUnitController> speedUnitController;
    std::shared_ptr<TrayConnectToDeviceController> trayConnectToDeviceController;
};
