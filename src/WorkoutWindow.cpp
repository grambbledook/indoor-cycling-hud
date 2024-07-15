#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"

#include <iostream>

#include "MetricsPanel.h"
#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"

WorkoutWindow::WorkoutWindow(QWidget *parent): AppWindow(parent) {
    const auto heart_rate_monitor_panel = new MetricsPanel(Constants::Labels::HEART_RATE, this);
    const auto cadence_sensor_panel = new MetricsPanel(Constants::Labels::CADENCE, this);
    const auto power_meter_panel = new MetricsPanel(Constants::Labels::INSTANT_POWER, this);
    const auto speed_sensor_panel = new MetricsPanel(Constants::Labels::SPEED, this);

    const auto finishLabel = new ButtonLabel(Constants::Buttons::FINISH, this);
    connect(finishLabel, &ButtonLabel::clicked, this, &WorkoutWindow::next);

    const auto layout = new QGridLayout(this);
    const auto centralWidget = new QWidget(this);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    layout->addWidget(heart_rate_monitor_panel, 0, 0);
    layout->addWidget(cadence_sensor_panel, 1, 0);
    layout->addWidget(power_meter_panel, 2, 0);
    layout->addWidget(speed_sensor_panel, 3, 0);
    layout->addWidget(finishLabel, 4, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void WorkoutWindow::next() {
    std::cout << "WorkoutWindow::next" << std::endl;
    emit nextScreen(Constants::Screens::WORKOUT_SUMMARY);
}
