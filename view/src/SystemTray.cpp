#include "SystemTray.h"

#include <QApplication>
#include <QMenu>
#include <QActionGroup>

#include "Constants.h"
#include "Units.h"
#include "WheelSizes.h"

SystemTray::SystemTray(
    const std::shared_ptr<ControllerHandler> &handler): QSystemTrayIcon(nullptr), handler(handler) {
    auto tray_icon = new QSystemTrayIcon(this);
    tray_icon->setIcon(QIcon("assets/hrm.png"));

    auto switch_theme = new QAction("Switch Theme", this);
    connect(switch_theme, &QAction::triggered, this, &SystemTray::switchTheme);

    auto quit_action = new QAction("Quit", this);
    connect(quit_action, &QAction::triggered, this, &SystemTray::quit);

    const auto wheel_size_menu = new QMenu("Wheel Size", nullptr);
    const auto wheel_size_group = new QActionGroup(this);
    wheel_size_group->setExclusive(true);

    for (int i = static_cast<int>(WheelSize::ROAD_700x18C); i <= static_cast<int>(WheelSize::ROAD_650x38A); ++i) {
        auto size = static_cast<WheelSize>(i);
        const auto action = new QAction(getWheelSizeString(size), this);
        action->setCheckable(true);
        if (size == WheelSize::ROAD_700x35C) {
            action->setChecked(true);
        }
        wheel_size_group->addAction(action);
        wheel_size_menu->addAction(action);
        connect(action, &QAction::triggered, this, [this, size]() { this->setWheelSize(size); });
    }

    const auto speed_unit_menu = new QMenu("Speed Unit", nullptr);
    const auto speed_unit_group = new QActionGroup(this);
    speed_unit_group->setExclusive(true);

    for (int i = static_cast<int>(SpeedUnit::KMH); i <= static_cast<int>(SpeedUnit::MPH); ++i) {
        auto unit = static_cast<SpeedUnit>(i);
        const auto action = new QAction(speedUnitToString(unit), this);
        action->setCheckable(true);
        if (unit == SpeedUnit::KMH) {
            action->setChecked(true);
        }
        speed_unit_group->addAction(action);
        speed_unit_menu->addAction(action);
        connect(action, &QAction::triggered, this, [this, unit]() { this->setSpeedUnit(unit); });
    }

    const auto tray_icon_menu = new QMenu();
    tray_icon_menu->addAction(switch_theme);
    tray_icon_menu->addMenu(wheel_size_menu);
    tray_icon_menu->addMenu(speed_unit_menu);
    tray_icon_menu->addAction(quit_action);
    tray_icon->setContextMenu(tray_icon_menu);
    tray_icon->show();
}

void SystemTray::switchTheme() const {
    handler->next(Constants::Commands::SWITCH_THEME);
}

void SystemTray::quit() const {
    handler->next(Constants::Commands::QUIT);
}

void SystemTray::setWheelSize(WheelSize size) const {
    handler->next(Constants::Commands::SET_WHEEL_SIZE);
}

void SystemTray::setSpeedUnit(SpeedUnit unit) const {
    handler->next(Constants::Commands::SET_WHEEL_SIZE);
}
