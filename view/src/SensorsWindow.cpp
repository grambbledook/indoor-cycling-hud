#include "StyleSheets.h"
#include "SensorsWindow.h"

#include <Constants.h>
#include <iostream>
#include <Labels.h>

#include <QGridLayout>

#include "Events.h"
#include "SupportedServices.h"


SensorsWindow::SensorsWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    eventHandlers.insert({
        getDeviceSelectedType(), [this](QEvent *event) {
            const auto device = dynamic_cast<DeviceSelectedEvent *>(event);
            deviceSelected(device->getEvent());
        }
    });

    eventHandlers.insert({
        getMeasurementReceivedType(), [this](QEvent *event) {
            const auto data = dynamic_cast<MeasurementReceivedEvent *>(event);
            std::cout << "SensorsWindow::MeasurementReceivedEvent" << std::endl;
            measurementsReceived(data->getData());
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

    const auto backLabel = new ButtonLabel(Constants::Buttons::BACK, true, this);
    connect(backLabel, &ButtonLabel::clicked, this, &SensorsWindow::back);

    const auto nextLabel = new ButtonLabel(Constants::Buttons::NEXT, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &SensorsWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(heartRateMonitorPanel, 0, 0, Qt::AlignCenter);
    layout->addWidget(cadencePanel, 0, 1, Qt::AlignCenter);
    layout->addWidget(speedPanel, 1, 0, Qt::AlignCenter);
    layout->addWidget(powerPanel, 1, 1, Qt::AlignCenter);

    layout->addWidget(backLabel, 3, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 3, 1, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void SensorsWindow::deviceSelected(const DeviceSelected &event) const {
    std::cout << "SensorsWindow::deviceSelected" << std::endl;

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
}

void SensorsWindow::measurementsReceived(const WorkoutData &measurements_update) const {
    heartRateMonitorPanel->measurementsReceived(measurements_update);
    cadencePanel->measurementsReceived(measurements_update);
    speedPanel->measurementsReceived(measurements_update);
    powerPanel->measurementsReceived(measurements_update);
}

void SensorsWindow::back() {
    controllerHandler->next(Constants::Screens::TRAINER);
}

void SensorsWindow::next() {
    controllerHandler->next(Constants::Screens::WORKOUT);
}
