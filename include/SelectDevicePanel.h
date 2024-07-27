#pragma once
#include <QMainWindow>
#include <QWidget>
#include "DeviceDialog.h"
#include "ControllerHandler.h"

class DeviceDialog;

class SelectDevicePanel final : public QMainWindow {
    Q_OBJECT

public:
    SelectDevicePanel(
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path,
        const std::shared_ptr<ControllerHandler> &handler,
        QWidget *parent = nullptr
    );

    struct Device {
        std::string name;
        std::string address;
    };

public slots:
    void handleDeviceButtonClick() const;

private:
    std::shared_ptr<DeviceDialog> dialog;
    std::shared_ptr<ControllerHandler> handler;
};
