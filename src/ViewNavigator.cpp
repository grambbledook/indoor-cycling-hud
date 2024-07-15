#include "ViewNavigator.h"

#include "Constants.h"

ViewNavigator::ViewNavigator(
    const std::shared_ptr<TrainerWindow> &trainer_window,
    const std::shared_ptr<SensorsWindow> &sensors_window,
    const std::shared_ptr<WorkoutWindow> &workout_window
): trainer_window(trainer_window), sensors_window(sensors_window), workout_window(workout_window), x(300), y(300) {
    connect(trainer_window.get(), &TrainerWindow::nextScreen, this, &ViewNavigator::nextScreen);
    connect(sensors_window.get(), &SensorsWindow::nextScreen, this, &ViewNavigator::nextScreen);
    connect(workout_window.get(), &WorkoutWindow::nextScreen, this, &ViewNavigator::nextScreen);

    connect(trainer_window.get(), &TrainerWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
    connect(sensors_window.get(), &SensorsWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
    connect(workout_window.get(), &WorkoutWindow::positionUpdated, this, &ViewNavigator::positionUpdate);
}

void ViewNavigator::nextScreen(const std::string &screen) const {
    if (screen == Constants::Screens::TRAINER) {
        sensors_window->hide();
        trainer_window->move(x, y);
        trainer_window->show();
    }

    if (screen == Constants::Screens::SENSORS) {
        trainer_window->hide();
        sensors_window->move(x, y);
        sensors_window->show();
    }

    if (screen == Constants::Screens::WORKOUT) {
        sensors_window->hide();
        workout_window->move(x, y);
        workout_window->show();
    }

    if (screen == Constants::Screens::WORKOUT_SUMMARY) {
        exit(0);
    }
}

void ViewNavigator::positionUpdate(const int x, const int y) {
    this->x = x;
    this->y = y;
}
