#include "SelectDevicePanel.h"
#include "AppStyleSheets.h"
#include "WorkoutWindow.h"

#include <spdlog/spdlog.h>

#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"
#include "Events.h"

WorkoutWindow::WorkoutWindow(
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
): AppWindow(handler, parent) {
    eventHandlers.insert({
        getWorkoutEventType(), [this](QEvent *event) {
            const auto data = dynamic_cast<WorkoutEventEvent *>(event);
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

auto WorkoutWindow::measurementsReceived(const WorkoutEvent &data) const -> void {
    heartRateMonitorPanel->measurementsReceived(data);
    cadenceSensorPanel->measurementsReceived(data);
    speedSensorPanel->measurementsReceived(data);
    powerMeterPanel->measurementsReceived(data);
}

auto WorkoutWindow::next() -> void {
    controllerHandler->next(Constants::Screens::WORKOUT_SUMMARY);
}
