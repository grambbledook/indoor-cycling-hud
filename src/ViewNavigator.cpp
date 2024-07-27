#include "ViewNavigator.h"

#include "Constants.h"
#include "DeviceDialog.h"

ViewNavigator::ViewNavigator(
    const std::shared_ptr<ControllerHandler> &controllerHandler,
    const std::shared_ptr<DeviceDialogController> &deviceDialogController,
    const std::shared_ptr<TrainerWindowController> &trainerWindowController,
    const std::shared_ptr<SensorsWindowController> &sensorsWindowController,
    const std::shared_ptr<WorkoutWindowController> &workoutWindowController
): controllerHandler(controllerHandler), deviceDialogController(deviceDialogController), trainerWindowController(trainerWindowController),
   sensorsWindowController(sensorsWindowController), workoutWindowController(workoutWindowController), x(300), y(300) {

    controllerHandler->subscribe([this](const std::string &screen) {
        this->nextScreen(screen);
    });
}

void ViewNavigator::nextScreen(const std::string &screen) const {
    if (screen == Constants::Screens::TRAINER) {
        trainerWindowController->handleRequest();
    }

    if (screen == Constants::Screens::SENSORS) {
        sensorsWindowController->handleRequest();
    }

    if (screen == Constants::Screens::WORKOUT) {
        workoutWindowController->handleRequest();
    }

    if (screen == Constants::Screens::DEVICE_DIALOG) {
        deviceDialogController->handleRequest();
    }

    if (screen == Constants::Screens::WORKOUT_SUMMARY) {
        exit(0);
    }
}
