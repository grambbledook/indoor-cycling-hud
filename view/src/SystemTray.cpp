#include "SystemTray.h"

#include <QApplication>
#include <QMenu>

#include "Constants.h"

SystemTray::SystemTray(
    const std::shared_ptr<ControllerHandler> &handler): QSystemTrayIcon(nullptr), handler(handler) {
    auto tray_icon = new QSystemTrayIcon(this);
    tray_icon->setIcon(QIcon("assets/hrm.png"));

    auto switch_theme = new QAction("Switch Theme", this);
    connect(switch_theme, &QAction::triggered, this, &SystemTray::switchTheme);

    auto quit_action = new QAction("Quit", this);
    connect(quit_action, &QAction::triggered, this, &SystemTray::quit);

    const auto tray_icon_menu = new QMenu();
    tray_icon_menu->addAction(switch_theme);
    tray_icon_menu->addAction(quit_action);
    tray_icon->setContextMenu(tray_icon_menu);
    tray_icon->show();
}

void SystemTray::switchTheme() {
    handler->next(Constants::Commands::SWITCH_THEME);
}

void SystemTray::quit() {
    handler->next(Constants::Commands::QUIT);
}
