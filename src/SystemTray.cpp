#include "SystemTray.h"

#include <QApplication>
#include <QMenu>

#include "BluetoothConstants.h"
#include "StyleSheets.h"

SystemTray::SystemTray(QWidget *parent): QSystemTrayIcon(parent), parent(parent) {
    auto tray_icon = new QSystemTrayIcon(this);
    tray_icon->setIcon(QIcon("assets/hrm.png"));

    auto switch_theme = new QAction("Switch Theme", this);
    connect(switch_theme, &QAction::triggered, this, &SystemTray::switchTheme);

    auto quit_action = new QAction("Quit", this);
    connect(quit_action, &QAction::triggered, this, &SystemTray::quit);

    auto tray_icon_menu = new QMenu();
    tray_icon_menu->addAction(switch_theme);
    tray_icon_menu->addAction(quit_action);
    tray_icon->setContextMenu(tray_icon_menu);
    tray_icon->show();
}

void SystemTray::switchTheme(bool checked) {
    static auto dark = StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM;
    static auto bright = StyleSheets::THEME_BRIGHT + StyleSheets::SCALE_MEDIUM;

    parent->setStyleSheet(checked ? dark.data() : bright.data());
}

void SystemTray::quit() {
    QApplication::quit();
}
