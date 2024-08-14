#include "ViewNavigator.h"

#include "Constants.h"
#include "DeviceDialog.h"

ViewNavigator::ViewNavigator(
    const std::shared_ptr<ControllerHandler> &controllerHandler,
    const std::shared_ptr<ShowDeviceDialogController> &deviceDialogController,
    const std::shared_ptr<ConnectToDeviceController> &connectToDeviceController,
    const std::shared_ptr<TrainerWindowController> &trainerWindowController,
    const std::shared_ptr<SensorsWindowController> &sensorsWindowController,
    const std::shared_ptr<SelectWorkoutWindowController> &selectWorkoutWindowController,
    const std::shared_ptr<WorkoutWindowController> &workoutWindowController,
    const std::shared_ptr<WorkoutSummaryWindowController> &workoutSummaryWindowController,
    const std::shared_ptr<SwitchThemeController> &switchThemeController,
    const std::shared_ptr<ShutdownController> &shutdownController
): connectToDeviceController(connectToDeviceController), controllerHandler(controllerHandler),
   deviceDialogController(deviceDialogController), trainerWindowController(trainerWindowController),
   sensorsWindowController(sensorsWindowController), shutdownController(shutdownController),
   switchThemeController(switchThemeController), workoutWindowController(workoutWindowController),
   workoutSummaryWindowController(workoutSummaryWindowController),
   selectWorkoutWindowController(selectWorkoutWindowController) {
    controllerHandler->subscribe([this](const std::string &screen) {
        this->nextScreen(screen);
    });
}

void ViewNavigator::nextScreen(const std::string &command) const {
    if (command == Constants::Screens::TRAINER) {
        trainerWindowController->handleRequest();
    }

    if (command == Constants::Screens::SENSORS) {
        sensorsWindowController->handleRequest();
    }

    if (command == Constants::Screens::SELECT_WORKOUT) {
        selectWorkoutWindowController->handleRequest();
    }

    if (command == Constants::Screens::WORKOUT) {
        workoutWindowController->handleRequest();
    }

    if (command == Constants::Screens::WORKOUT_SUMMARY) {
        workoutSummaryWindowController->handleRequest();
    }

    if (command == Constants::Screens::DEVICE_DIALOG) {
        deviceDialogController->handleRequest();
    }

    if (command == Constants::Commands::CONNECT) {
        connectToDeviceController->handleRequest();
    }

    if (command == Constants::Commands::SWITCH_THEME) {
        switchThemeController->handleRequest();
    }

    if (command == Constants::Commands::QUIT) {
        shutdownController->handleRequest();
    }
}
