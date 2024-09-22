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
    auto switchTheme() const -> void;

    auto quit() const -> void;

    auto setWheelSize(WheelSize size) const -> void;

    auto setSpeedUnit(DistanceUnit size) const -> void;

    auto addDevice(const std::shared_ptr<Device> &device) -> void;

    auto handleSetDevice(const std::shared_ptr<Device> &device) const -> void;

    auto event(QEvent *event) -> bool;

private:
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;

    bool darkThemeEvanled = false;
    std::shared_ptr<ControllerHandler> handler;

    QMenu *device_menu;
    QActionGroup *device_group;
};
