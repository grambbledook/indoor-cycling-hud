#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"
#include "MetricsPanel.h"
#include <Labels.h>

#include <QGridLayout>

WorkoutWindow::WorkoutWindow(QWidget *parent): AppWindow(parent) {
    const auto heart_rate_monitor_panel = new MetricsPanel("Heart Rate", this);
    const auto cadence_sensor_panel = new MetricsPanel("Cadence", this);
    const auto power_meter_panel = new MetricsPanel("Instant Power", this);
    const auto speed_sensor_panel = new MetricsPanel("Speed", this);

    const auto finishLabel = new ButtonLabel("Finish", this);

    const auto layout = new QGridLayout(this);
    const auto centralWidget = new QWidget(this);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    layout->addWidget(heart_rate_monitor_panel, 0, 0);
    layout->addWidget(cadence_sensor_panel, 1, 0);
    layout->addWidget(power_meter_panel, 2, 0);
    layout->addWidget(speed_sensor_panel, 3, 0);
    layout->addWidget(finishLabel, 4, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);

    setStyleSheet((StyleSheets::THEME_BRIGHT + StyleSheets::SCALE_MEDIUM).data());
}
