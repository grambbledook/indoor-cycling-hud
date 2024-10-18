#pragma once
#include <QActionGroup>
#include <QSystemTrayIcon>
#include <qobject>
#include <qtmetamacros.h>

#include "ControllerHandler.h"
#include "Data.h"
#include "Units.h"
#include "WheelSizes.h"

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
    bool darkThemeEvanled = false;
    std::shared_ptr<ControllerHandler> handler;

    QMenu *device_menu;
    QActionGroup *device_group;
};
