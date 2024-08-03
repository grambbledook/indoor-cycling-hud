#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"

#include <iostream>

#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"
#include "Events.h"


WorkoutWindow::WorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    eventHandlers.insert({
        getMeasurementReceivedType(), [this](QEvent *event) {
            const auto data = dynamic_cast<MeasurementReceivedEvent *>(event);
            std::cout << "WorkoutWindow::MeasurementReceivedEvent" << std::endl;
            measurementsReceived(data->getData());
        }
    });

    heartRateMonitorPanel = new MetricsPanel(Data::HEART_RATE, this);
    cadenceSensorPanel = new MetricsPanel(Data::CADENCE, this);
    powerMeterPanel = new MetricsPanel(Data::SPEED, this);
    speedSensorPanel = new MetricsPanel(Data::POWER, this);

    const auto finishLabel = new ButtonLabel(Constants::Buttons::FINISH, this);
    connect(finishLabel, &ButtonLabel::clicked, this, &WorkoutWindow::next);

    const auto layout = new QGridLayout(this);
    const auto centralWidget = new QWidget(this);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    layout->addWidget(heartRateMonitorPanel, 0, 0);
    layout->addWidget(cadenceSensorPanel, 1, 0);
    layout->addWidget(powerMeterPanel, 2, 0);
    layout->addWidget(speedSensorPanel, 3, 0);
    layout->addWidget(finishLabel, 4, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);
}

void WorkoutWindow::measurementsReceived(const WorkoutData &data) {
    heartRateMonitorPanel->measurementsReceived(data);
    cadenceSensorPanel->measurementsReceived(data);
    speedSensorPanel->measurementsReceived(data);
    powerMeterPanel->measurementsReceived(data);
}

void WorkoutWindow::next() {
    std::cout << "WorkoutWindow::next" << std::endl;
    controllerHandler->next(Constants::Commands::QUIT);
}
