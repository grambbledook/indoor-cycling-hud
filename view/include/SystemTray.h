#pragma once
#include <QSystemTrayIcon>
#include <qobject>
#include <qtmetamacros.h>

#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "Units.h"
#include "WheelSizes.h"
#include "WorkoutWindow.h"

class SystemTray final : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit SystemTray(
        const std::shared_ptr<ControllerHandler> &handler);

public slots:
    void switchTheme() const;

    void quit() const;

    void setWheelSize(WheelSize size) const;

    void setSpeedUnit(SpeedUnit size) const;

private:
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;

    bool darkThemeEvanled = false;
    std::shared_ptr<ControllerHandler> handler;
};
