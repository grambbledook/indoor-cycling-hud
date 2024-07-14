#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QDialog>

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

    //
public slots:
    void show_device_dialog();

    //
private:
    std::shared_ptr<DeviceDialog> dialog;

    //     QString highlighted_icon_path;
    //     QString normal_icon_path;
    // ClickableLabel *selectIcon;
    // ValueLabel *metricLabel;
};
