#pragma once
#include <QMainWindow>
#include <QWidget>
#include "ControllerHandler.h"

#include "Labels.h"
#include "Data.h"
#include "ModelEvents.h"
#include "SupportedServices.h"
class DeviceDialog;

class SelectDevicePanel final : public QMainWindow {
    Q_OBJECT

public:
    SelectDevicePanel(
        const AppService &service,
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path,
        const std::shared_ptr<ControllerHandler> &handler,
        QWidget *parent = nullptr
    );

    auto deviceSelected(const DeviceSelected &event) -> void;
    auto measurementsReceived(const WorkoutEvent &measurements_update) const -> void;

public slots:
    auto handleDeviceButtonClick() const -> void;

private:
    const AppService &service;

    std::shared_ptr<ControllerHandler> handler;
    ClickableLabel *selectIcon;
    ValueLabel *metricLabel;
};
