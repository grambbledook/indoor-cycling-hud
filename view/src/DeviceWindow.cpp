#include "DeviceWindow.h"


#include <Constants.h>
#include <spdlog/spdlog.h>
#include <Labels.h>

#include <QGridLayout>

#include "Events.h"
#include "SupportedServices.h"

DeviceWindow::DeviceWindow(
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
): AppWindow(handler, parent) {
    eventHandlers.insert({
        getDeviceSelectedType(), [this](QEvent *event) {
            const auto device = dynamic_cast<DeviceSelectedEvent *>(event);
            deviceSelected(device->getEvent());
        }
    });

    heartRateMonitorPanel = new SelectDevicePanel(
        HEART_RATE,
        Constants::Icons::HEART_RATE_MONITOR,
        Constants::Icons::HEART_RATE_MONITOR_HOVER,
        handler, this
    );

    cadencePanel = new SelectDevicePanel(
        CADENCE,
        Constants::Icons::CADENCE_SENSOR,
        Constants::Icons::CADENCE_SENSOR_HOVER,
        handler, this
    );

    speedPanel = new SelectDevicePanel(
        SPEED,
        Constants::Icons::SPEED_SENSOR,
        Constants::Icons::SPEED_SENSOR_HOVER,
        handler, this
    );

    powerPanel = new SelectDevicePanel(
        POWER,
        Constants::Icons::POWER_SENSOR,
        Constants::Icons::POWER_SENSOR_HOVER,
        handler, this
    );

    trainerPanel = new SelectDevicePanel(
        BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER_HOVER,
        handler, this
    );

    const auto nextLabel = new ButtonLabel(Constants::Buttons::NEXT, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &DeviceWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(heartRateMonitorPanel, 0, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 0, 1, Qt::AlignCenter);
    layout->addWidget(cadencePanel, 0, 2, Qt::AlignCenter);

    layout->addWidget(powerPanel, 1, 0, Qt::AlignCenter);
    layout->addWidget(trainerPanel, 1, 1, Qt::AlignCenter);
    layout->addWidget(speedPanel, 1, 2, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

auto DeviceWindow::deviceSelected(const DeviceSelected &event) const -> void {
    spdlog::info("SensorsWindow::deviceSelected");

    if (event.service == Service::HEART_RATE) {
        heartRateMonitorPanel->deviceSelected(event);
    }

    if (event.service == Service::CADENCE) {
        cadencePanel->deviceSelected(event);
    }

    if (event.service == Service::SPEED) {
        speedPanel->deviceSelected(event);
    }

    if (event.service == Service::POWER) {
        powerPanel->deviceSelected(event);
    }

    if (event.service == Service::BIKE_TRAINER) {
        trainerPanel->deviceSelected(event);
    }
}

auto DeviceWindow::back() -> void {
}

auto DeviceWindow::next() -> void {
    controllerHandler->next(Constants::Screens::SELECT_WORKOUT);
}
