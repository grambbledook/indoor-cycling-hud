#include "ViewNavigator.h"
#include "Constants.h"

ViewNavigator::ViewNavigator(
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
): controllerHandler(controllerHandler), connectToDeviceController(connectToDeviceController),
   deviceDialogController(deviceDialogController), deviceWindowController(deviceWindowController),
   selectWorkoutWindowController(selectWorkoutWindowController), workoutWindowController(workoutWindowController),
   workoutSummaryWindowController(workoutSummaryWindowController), switchThemeController(switchThemeController),
   shutdownController(shutdownController), wheelSizeSelectionController(wheelSizeSelectionController),
   speedUnitController(speedUnitController) {
    auto receiver = [this](const std::string &screen, const std::vector<std::any> &args) {
        this->nextScreen(screen, args);
    };
    controllerHandler->subscribe(receiver);
}

auto ViewNavigator::nextScreen(const std::string &command, const std::vector<std::any> &args) const -> void {
    if (command == Constants::Screens::SENSORS) {
        deviceWindowController->showDeviceWindow();
    }

    if (command == Constants::Screens::SELECT_WORKOUT) {
        selectWorkoutWindowController->showSelectWorkoutWindow();
    }

    if (command == Constants::Screens::WORKOUT) {
        workoutWindowController->showWorkoutWindow();
    }

    if (command == Constants::Screens::WORKOUT_SUMMARY) {
        workoutSummaryWindowController->showWorkoutSummaryWindow();
    }

    if (command == Constants::Screens::DEVICE_DIALOG_SHOW) {
        deviceDialogController->showDialog();
    }

    if (command == Constants::Screens::DEVICE_DIALOG_CLOSE) {
        deviceDialogController->closeDialog();
    }

    if (command == Constants::Commands::SWITCH_THEME) {
        switchThemeController->switchColorTheme();
    }

    if (command == Constants::Commands::QUIT) {
        shutdownController->shutdown();
    }

    if (command == Constants::Commands::SET_WHEEL_SIZE) {
        assert(!args.empty() && "wheel size is required");

        const auto wheelSize = std::any_cast<WheelSize>(args[0]);
        wheelSizeSelectionController->setWheelSize(wheelSize);
    }

    if (command == Constants::Commands::SET_SPEED_UNIT) {
        assert(!args.empty() && "speed unit is required");

        const auto speedUnit = std::any_cast<DistanceUnit>(args[0]);
        speedUnitController->setDistanceUnit(speedUnit);
    }

    if (command == Constants::Commands::CONNECT_TO_DEVICE) {
        assert(!args.empty() && "device is required");

        const auto device = std::any_cast<const std::shared_ptr<Device> &>(args[0]);
        connectToDeviceController->connectToDevice(device);
    }
}
