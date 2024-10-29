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
        const std::shared_ptr<DeviceDialogController> &deviceDialogController,
        const std::shared_ptr<ConnectToDeviceController> &connectToDeviceController,
        const std::shared_ptr<DeviceWindowController> &deviceWindowController,
        const std::shared_ptr<SelectWorkoutWindowController> &selectWorkoutWindowController,
        const std::shared_ptr<WorkoutWindowController> &workoutWindowController,
        const std::shared_ptr<WorkoutSummaryWindowController> &workoutSummaryWindowController,
        const std::shared_ptr<SwitchThemeController> &switchThemeController,
        const std::shared_ptr<ShutdownController> &shutdownController,
        const std::shared_ptr<WheelSizeSelectionController> &wheelSizeSelectionController,
        const std::shared_ptr<SpeedUnitController> &speedUnitController
    );

public slots:
    auto nextScreen(const std::string &command, const std::vector<std::any> &args = {}) const -> void;

private:
    std::shared_ptr<ControllerHandler> controllerHandler;
    std::shared_ptr<ConnectToDeviceController> connectToDeviceController;
    std::shared_ptr<DeviceDialogController> deviceDialogController;
    std::shared_ptr<DeviceWindowController> deviceWindowController;
    std::shared_ptr<SelectWorkoutWindowController> selectWorkoutWindowController;
    std::shared_ptr<WorkoutWindowController> workoutWindowController;
    std::shared_ptr<WorkoutSummaryWindowController> workoutSummaryWindowController;
    std::shared_ptr<SwitchThemeController> switchThemeController;
    std::shared_ptr<ShutdownController> shutdownController;
    std::shared_ptr<WheelSizeSelectionController> wheelSizeSelectionController;
    std::shared_ptr<SpeedUnitController> speedUnitController;
};
