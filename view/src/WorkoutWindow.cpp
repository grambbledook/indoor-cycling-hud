#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"

#include <spdlog/spdlog.h>

#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"
#include "Events.h"

WorkoutWindow::WorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    eventHandlers.insert({
        getWorkoutDataType(), [this](QEvent *event) {
            const auto data = dynamic_cast<WorkoutDataEvent *>(event);
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
    centralWidget->setObjectName(Constants::Classes::PANEL);
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
    spdlog::info("WorkoutWindow::next");
    controllerHandler->next(Constants::Screens::WORKOUT_SUMMARY);
}