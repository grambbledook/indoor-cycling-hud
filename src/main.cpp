#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QtPlugin>

#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"

int main(int argc, char **argv) {
    const auto app = new QApplication(argc, argv);

    const auto trainer_window = new TrainerWindow();
    trainer_window->show();

    const auto sensors_window = new SensorsWindow();
    sensors_window->show();

    const auto workout_window = new WorkoutWindow();
    workout_window->show();

    return app->exec();
}
