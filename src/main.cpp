#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QtPlugin>

#include "Constants.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "ViewNavigator.h"
#include "WorkoutWindow.h"

int main(int argc, char **argv) {
    const auto app = new QApplication(argc, argv);

    auto trainer_window = std::make_shared<TrainerWindow>();
    auto sensor_window = std::make_shared<SensorsWindow>();
    auto workout_window = std::make_shared<WorkoutWindow>();

    const auto view_navigator = std::make_unique<ViewNavigator>(trainer_window, sensor_window, workout_window);
    view_navigator->nextScreen(Constants::Screens::TRAINER);

    return app->exec();
}
