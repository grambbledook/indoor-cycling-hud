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
        const auto action = new QAction(toWheelSizeString(size), this);
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

    for (int i = static_cast<int>(DistanceUnit::METERS); i <= static_cast<int>(DistanceUnit::MILES); ++i) {
        auto unit = static_cast<DistanceUnit>(i);
        const auto action = new QAction(distanceUnitToString(unit), this);
        action->setCheckable(true);
        if (unit == DistanceUnit::METERS) {
            action->setChecked(true);
        }
        speed_unit_group->addAction(action);
        speed_unit_menu->addAction(action);
        connect(action, &QAction::triggered, this, [this, unit]() { this->setSpeedUnit(unit); });
    }

    device_menu = new QMenu("Devices", nullptr);
    device_group = new QActionGroup(this);

    const auto tray_icon_menu = new QMenu();
    tray_icon_menu->addAction(switch_theme);
    tray_icon_menu->addMenu(device_menu);
    tray_icon_menu->addMenu(wheel_size_menu);
    tray_icon_menu->addMenu(speed_unit_menu);
    tray_icon_menu->addAction(quit_action);
    tray_icon->setContextMenu(tray_icon_menu);
    tray_icon->show();
}

auto SystemTray::switchTheme() const -> void {
    handler->next(Constants::Commands::SWITCH_THEME);
}

auto SystemTray::quit() const -> void {
    handler->next(Constants::Commands::QUIT);
}

auto SystemTray::setWheelSize(const WheelSize size) const -> void {
    handler->next(Constants::Commands::SET_WHEEL_SIZE, size);
}

auto SystemTray::setSpeedUnit(const DistanceUnit unit) const -> void {
    handler->next(Constants::Commands::SET_SPEED_UNIT, unit);
}

auto SystemTray::addDevice(const std::shared_ptr<Device> &device) -> void {
    for (const auto action: device_group->actions()) {
        if (action->data().value<std::shared_ptr<Device> >()->deviceId() == device->deviceId()) {
            return;
        }
    }

    const auto action = new QAction(QString::fromStdString(device->name.value), this);
    action->setData(QVariant::fromValue(device));
    device_group->addAction(action);
    device_menu->addAction(action);

    connect(action, &QAction::triggered, this, [this, device] { this->handleSetDevice(device); });
}

auto SystemTray::handleSetDevice(const std::shared_ptr<Device> &device) const -> void {
    handler->next(Constants::Commands::CONNECT_TO_DEVICE, device);
}

auto SystemTray::event(QEvent *event) -> bool {
    if (event->type() > QEvent::MaxUser or event->type() < QEvent::User) {
        return QSystemTrayIcon::event(event);
    }

    const auto deviceEvent = dynamic_cast<DeviceDiscoveredEvent *>(event);
    if (not deviceEvent) {
        return QSystemTrayIcon::event(event);
    }

    const std::shared_ptr<Device> device = deviceEvent->getEvent().device;
    addDevice(device);
    return true;
}
