
#include "StyleSheets.h"
#include "SensorsWindow.h"

#include <Constants.h>
#include <iostream>
#include <Labels.h>

#include <QGridLayout>

#include "Service.h"

SensorsWindow::SensorsWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    heartRateMonitorPanel = new SelectDevicePanel(
        &Services::HRM,
        Constants::Icons::HEART_RATE_MONITOR,
        Constants::Icons::HEART_RATE_MONITOR_HOVER,
        handler, this
    );

     cadencePanel = new SelectDevicePanel(
        &Services::CSC,
        Constants::Icons::CADENCE_SENSOR,
        Constants::Icons::CADENCE_SENSOR_HOVER,
        handler, this
    );

     speedPanel = new SelectDevicePanel(
        &Services::CSC,
        Constants::Icons::SPEED_SENSOR,
        Constants::Icons::SPEED_SENSOR_HOVER,
        handler, this
    );

     powerPanel = new SelectDevicePanel(
        &Services::PWR,
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
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void SensorsWindow::deviceSelected(const std::shared_ptr<Device> &device) {
    std::cout << "SensorsWindow::deviceSelected" << std::endl;

    heartRateMonitorPanel->deviceSelected(device);
    cadencePanel->deviceSelected(device);
    speedPanel->deviceSelected(device);
    powerPanel->deviceSelected(device);
}

void SensorsWindow::back() {
    std::cout << "SensorsWindow::back" << std::endl;
    const auto x = this->x(), y = this->y();
    controllerHandler->next(Constants::Screens::TRAINER);
}

void SensorsWindow::next() {
    std::cout << "SensorsWindow::next" << std::endl;
    const auto x = this->x(), y = this->y();
    controllerHandler->next(Constants::Screens::WORKOUT);
}
