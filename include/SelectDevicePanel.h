#pragma once
#include <QMainWindow>
#include <QWidget>
#include "DeviceDialog.h"
#include "ControllerHandler.h"

#include "Labels.h"
#include "Stats.h"

class DeviceDialog;

class SelectDevicePanel final : public QMainWindow {
    Q_OBJECT

public:
    SelectDevicePanel(
        const GattService *service,
        const std::string &what,
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path,
        const std::shared_ptr<ControllerHandler> &handler,
        QWidget *parent = nullptr
    );

public:
    void deviceSelected(const std::shared_ptr<Device> &device);
    void measurementsReceived(const MeasurementsUpdate & measurements_update);

public slots:
    void handleDeviceButtonClick() const;

private:
    const GattService *service;

    std::shared_ptr<ControllerHandler> handler;
    ClickableLabel *selectIcon;
    ValueLabel *metricLabel;
    std::string what;
};
