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

    eventHandlers.insert({
        getSubscribedToServiceType(), [this](QEvent *event) {
            const auto device = dynamic_cast<SubscribedToServiceEvent *>(event);
            subscribedToService(device->getEvent());
        }
    });

    heartRateMonitorPanel = new SelectDevicePanel(
        HEART_RATE,
        Constants::Icons::HEART_RATE_MONITOR,
        Constants::Icons::HEART_RATE_MONITOR_HOVER,
        handler, LabelSize::SMALL, this
    );

    cadencePanel = new SelectDevicePanel(
        CADENCE,
        Constants::Icons::CADENCE_SENSOR,
        Constants::Icons::CADENCE_SENSOR_HOVER,
        handler, LabelSize::SMALL, this
    );

    speedPanel = new SelectDevicePanel(
        SPEED,
        Constants::Icons::SPEED_SENSOR,
        Constants::Icons::SPEED_SENSOR_HOVER,
        handler, LabelSize::SMALL, this
    );

    powerPanel = new SelectDevicePanel(
        POWER,
        Constants::Icons::POWER_SENSOR,
        Constants::Icons::POWER_SENSOR_HOVER,
        handler, LabelSize::SMALL, this
    );

    trainerPanel = new SelectDevicePanel(
        BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER_HOVER,
        handler, LabelSize::MEDIUM, this
    );

    const auto startLabel = new ButtonLabel(Constants::Buttons::START, true, this);
    connect(startLabel, &ButtonLabel::clicked, this, &DeviceWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(trainerPanel, 0, 1, 1, 2, Qt::AlignCenter);

    layout->addWidget(heartRateMonitorPanel, 1, 0, Qt::AlignCenter);
    layout->addWidget(speedPanel, 1, 1, Qt::AlignCenter);
    layout->addWidget(cadencePanel, 1, 2, Qt::AlignCenter);
    layout->addWidget(powerPanel, 1, 3, Qt::AlignCenter);

    layout->addWidget(startLabel, 2, 1, 1, 2, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

auto DeviceWindow::deviceSelected(const DeviceSelected &event) const -> void {
    spdlog::info("DeviceWindow::deviceSelected");

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

auto DeviceWindow::subscribedToService(const SubscribedToService &event) const -> void {
    spdlog::info("DeviceWindow::subscribedToService");

    if (event.service == Service::HEART_RATE) {
        heartRateMonitorPanel->subscribedToService(event);
    }

    if (event.service == Service::CADENCE) {
        cadencePanel->subscribedToService(event);
    }

    if (event.service == Service::SPEED) {
        speedPanel->subscribedToService(event);
    }

    if (event.service == Service::POWER) {
        powerPanel->subscribedToService(event);
    }

    if (event.service == Service::BIKE_TRAINER) {
        trainerPanel->subscribedToService(event);
    }
}

auto DeviceWindow::back() -> void {
}

auto DeviceWindow::next() -> void {
    controllerHandler->next(Constants::Screens::WORKOUT);
}