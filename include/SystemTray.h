#pragma once
#include <QSystemTrayIcon>
#include <qobject>
#include <qtmetamacros.h>

#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"

class SystemTray final : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit SystemTray(
        const std::shared_ptr<ControllerHandler> &handler);

public slots:
    void switchTheme();

    void quit();

private:
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;

    bool darkThemeEvanled = false;
    std::shared_ptr<ControllerHandler> handler;
};
