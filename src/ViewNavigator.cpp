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

    // connect(trainerWindowController->view.get(), &TrainerWindow::nextScreen, this, &ViewNavigator::nextScreen);
    // connect(sensorsWindowController->view.get(), &SensorsWindow::nextScreen, this, &ViewNavigator::nextScreen);
    // connect(workoutWindowController->view.get(), &WorkoutWindow::nextScreen, this, &ViewNavigator::nextScreen);

    connect(trainerWindowController->view.get(), &TrainerWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
    connect(sensorsWindowController->view.get(), &SensorsWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
    connect(workoutWindowController->view.get(), &WorkoutWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
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

void ViewNavigator::positionUpdate(const int x, const int y) {
    this->x = x;
    this->y = y;
}
