#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "SensorsWindow.h"

#include <Constants.h>
#include <Labels.h>

#include <QGridLayout>

SensorsWindow::SensorsWindow(QWidget *parent): AppWindow(parent) {
    const auto heart_rate_monitor_panel = new SelectDevicePanel(
        Constants::Icons::HEART_RATE_MONITOR,
        Constants::Icons::HEART_RATE_MONITOR_HOVER,
        this
    );

    const auto cadence_panel = new SelectDevicePanel(
        Constants::Icons::CADENCE_SENSOR,
        Constants::Icons::CADENCE_SENSOR_HOVER,
        this
    );

    const auto speed_panel = new SelectDevicePanel(
        Constants::Icons::SPEED_SENSOR,
        Constants::Icons::SPEED_SENSOR_HOVER,
        this
    );

    const auto power_panel = new SelectDevicePanel(
        Constants::Icons::POWER_SENSOR,
        Constants::Icons::POWER_SENSOR_HOVER,
        this
    );

    const auto prevLabel = new ButtonLabel("Prev", true, this);
    const auto nextLabel = new ButtonLabel("Next", true, this);

    auto *layout = new QGridLayout(this);
    layout->addWidget(heart_rate_monitor_panel, 0, 0, Qt::AlignCenter);
    layout->addWidget(cadence_panel, 0, 1, Qt::AlignCenter);
    layout->addWidget(speed_panel, 1, 0, Qt::AlignCenter);
    layout->addWidget(power_panel, 1, 1, Qt::AlignCenter);

    layout->addWidget(prevLabel, 3, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 3, 1, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_BRIGHT + StyleSheets::SCALE_MEDIUM).data());
}
