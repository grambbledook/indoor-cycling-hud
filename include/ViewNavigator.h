#pragma once

#include <TrainerWindow.h>
#include <SensorsWindow.h>
#include <WorkoutWindow.h>


class ViewNavigator final: public QObject {
    Q_OBJECT
public:
    explicit ViewNavigator(
        const std::shared_ptr<TrainerWindow> &trainer_window,
        const std::shared_ptr<SensorsWindow> &sensors_window,
        const std::shared_ptr<WorkoutWindow> &workout_window
    );

public slots:
    void nextScreen(const std::string &screen) const;
    void positionUpdate(int x, int y);

private:
    std::shared_ptr<TrainerWindow> trainer_window;
    std::shared_ptr<SensorsWindow> sensors_window;
    std::shared_ptr<WorkoutWindow> workout_window;

    int x;
    int y;
};
