#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QtPlugin>

#include "Constants.h"
#include "Controller.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "ViewNavigator.h"
#include "WorkoutWindow.h"

#include "WinRT.h"

int main(int argc, char **argv) {
    const auto app = new QApplication(argc, argv);

    auto appState = std::make_shared<AppState>();
    auto history = std::make_shared<std::stack<std::shared_ptr<QWidget> > >();

    auto model = std::make_shared<Model>();

    auto controllerHandler = std::make_shared<ControllerHandler>();

    auto deviceDialog = [&controllerHandler](QWidget *parent) {
        return std::make_shared<DeviceDialog>(controllerHandler, parent);
    };

    auto deviceDialogController = std::make_shared<DeviceDialogController>(
        deviceDialog, appState, history, model);

    auto trainerWindowController = std::make_shared<TrainerWindowController>(
        std::make_shared<TrainerWindow>(controllerHandler), appState, history, model);

    auto sensorWindowController = std::make_shared<SensorsWindowController>(
        std::make_shared<SensorsWindow>(controllerHandler), appState, history, model);

    auto workoutWindowController = std::make_shared<WorkoutWindowController>(
        std::make_shared<WorkoutWindow>(controllerHandler), appState, history, model);

    const auto view_navigator = std::make_unique<ViewNavigator>(
        controllerHandler,
        deviceDialogController, trainerWindowController, sensorWindowController, workoutWindowController
    );

    view_navigator->nextScreen(Constants::Screens::TRAINER);

    return app->exec();
}
