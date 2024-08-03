#pragma once
#include <QSystemTrayIcon>
#include <qobject>
#include <qtmetamacros.h>

class SystemTray final : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit SystemTray(QWidget *parent = nullptr);

public slots:
    void switchTheme(bool checked);

    void quit();

private:
    QWidget *parent;
};
