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

    void setSpeedUnit(DistanceUnit size) const;

    void addDevice(const std::shared_ptr<Device> &device);
    void handleSetDevice(const std::shared_ptr<Device> &device) const;

    bool event(QEvent *event);

private:
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;

    bool darkThemeEvanled = false;
    std::shared_ptr<ControllerHandler> handler;

    QMenu *device_menu;
    QActionGroup *device_group;
};
