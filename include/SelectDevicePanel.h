#pragma once
#include <QMainWindow>
#include <QWidget>
#include "DeviceDialog.h"

class DeviceDialog;

class SelectDevicePanel final : public QMainWindow {
    Q_OBJECT

public:
    SelectDevicePanel(
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path,
        QWidget *parent = nullptr
    );

    struct Device {
        std::string name;
        std::string address;
    };

public slots:
    void showDeviceDialog();

private:
    std::shared_ptr<DeviceDialog> dialog;
};
