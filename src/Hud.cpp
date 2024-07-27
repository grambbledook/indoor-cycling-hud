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
    WinRT::test(true);

    const auto app = new QApplication(argc, argv);

    auto appState = std::make_shared<AppState>();
    auto history = std::make_shared<std::stack<std::shared_ptr<QWidget> > >();

    auto deviceDialogController = std::make_shared<DeviceDialogController>(appState, history);
    auto trainerWindowController = std::make_shared<TrainerWindowController>(std::make_shared<TrainerWindow>(), appState, history);
    auto sensorWindowController = std::make_shared<SensorsWindowController>(std::make_shared<SensorsWindow>(), appState, history);
    auto workoutWindowController = std::make_shared<WorkoutWindowController>(std::make_shared<WorkoutWindow>(), appState, history);

    const auto view_navigator = std::make_unique<ViewNavigator>(
        deviceDialogController, trainerWindowController, sensorWindowController, workoutWindowController
        );

    view_navigator->nextScreen(Constants::Screens::TRAINER);

    return app->exec();
}
