#include "WorkoutSummaryPanel.h"

#include <Constants.h>

#include <spdlog/spdlog.h>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "DataFields.h"

WorkoutSummaryPanel::WorkoutSummaryPanel(QWidget *parent): QMainWindow(parent) {
    // Duration Group
    auto *durationLayout = new QHBoxLayout();
    durationField = new TextLabel(Constants::Labels::DURATION, LabelSize::SMALL, this);
    durationValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    durationLayout->addWidget(durationField, 0, Qt::AlignLeft);
    durationLayout->addItem(new QSpacerItem(20, 0));
    durationLayout->addWidget(durationValue, 2, Qt::AlignRight);

    auto *distanceLayout = new QHBoxLayout();
    distanceField = new TextLabel(Constants::Labels::DISTANCE, LabelSize::SMALL, this);
    distanceValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    distanceLayout->addWidget(distanceField, 0, Qt::AlignLeft);
    distanceLayout->addItem(new QSpacerItem(20, 0));
    distanceLayout->addWidget(distanceValue, 2, Qt::AlignRight);

    // HRM Group
    auto *hrmLayout = new QGridLayout();
    maxHrmField = new TextLabel(Constants::Labels::MAX_HEART_RATE, LabelSize::SMALL, this);
    maxHrmValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    minHrmField = new TextLabel(Constants::Labels::MIN_HEART_RATE, LabelSize::SMALL, this);
    minHrmValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    avgHrmField = new TextLabel(Constants::Labels::AVG_HEART_RATE, LabelSize::SMALL, this);
    avgHrmValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    hrmLayout->addWidget(maxHrmField, 0, 0, Qt::AlignLeft);
    hrmLayout->addWidget(maxHrmValue, 0, 1, Qt::AlignRight);
    hrmLayout->addWidget(minHrmField, 1, 0, Qt::AlignLeft);
    hrmLayout->addWidget(minHrmValue, 1, 1, Qt::AlignRight);
    hrmLayout->addWidget(avgHrmField, 2, 0, Qt::AlignLeft);
    hrmLayout->addWidget(avgHrmValue, 2, 1, Qt::AlignRight);

    // Cadence Group
    auto *cadenceLayout = new QGridLayout();
    maxCadenceField = new TextLabel(Constants::Labels::MAX_CADENCE, LabelSize::SMALL, this);
    maxCadenceValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    minCadenceField = new TextLabel(Constants::Labels::MIN_CADENCE, LabelSize::SMALL, this);
    minCadenceValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    avgCadenceField = new TextLabel(Constants::Labels::AVG_CADENCE, LabelSize::SMALL, this);
    avgCadenceValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    cadenceLayout->addWidget(maxCadenceField, 0, 0, Qt::AlignLeft);
    cadenceLayout->addWidget(maxCadenceValue, 0, 1, Qt::AlignRight);
    cadenceLayout->addWidget(minCadenceField, 1, 0, Qt::AlignLeft);
    cadenceLayout->addWidget(minCadenceValue, 1, 1, Qt::AlignRight);
    cadenceLayout->addWidget(avgCadenceField, 2, 0, Qt::AlignLeft);
    cadenceLayout->addWidget(avgCadenceValue, 2, 1, Qt::AlignRight);

    // Speed Group
    auto *speedLayout = new QGridLayout();
    maxSpeedField = new TextLabel(Constants::Labels::MAX_SPEED, LabelSize::SMALL, this);
    maxSpeedValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    minSpeedField = new TextLabel(Constants::Labels::MIN_SPEED, LabelSize::SMALL, this);
    minSpeedValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    avgSpeedField = new TextLabel(Constants::Labels::AVG_SPEED, LabelSize::SMALL, this);
    avgSpeedValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    speedLayout->addWidget(maxSpeedField, 0, 0, Qt::AlignLeft);
    speedLayout->addWidget(maxSpeedValue, 0, 1, Qt::AlignRight);
    speedLayout->addWidget(minSpeedField, 1, 0, Qt::AlignLeft);
    speedLayout->addWidget(minSpeedValue, 1, 1, Qt::AlignRight);
    speedLayout->addWidget(avgSpeedField, 2, 0, Qt::AlignLeft);
    speedLayout->addWidget(avgSpeedValue, 2, 1, Qt::AlignRight);

    // Power Group
    auto *powerLayout = new QGridLayout();
    maxPowerField = new TextLabel(Constants::Labels::MAX_POWER, LabelSize::SMALL, this);
    maxPowerValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    minPowerField = new TextLabel(Constants::Labels::MIN_POWER, LabelSize::SMALL, this);
    minPowerValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);
    avgPowerField = new TextLabel(Constants::Labels::AVG_POWER, LabelSize::SMALL, this);
    avgPowerValue = new ValueLabel(Constants::Values::EMPTY_DATA, LabelSize::SMALL, this);

    powerLayout->addWidget(maxPowerField, 0, 0, Qt::AlignLeft);
    powerLayout->addWidget(maxPowerValue, 0, 1, Qt::AlignRight);
    powerLayout->addWidget(minPowerField, 1, 0, Qt::AlignLeft);
    powerLayout->addWidget(minPowerValue, 1, 1, Qt::AlignRight);
    powerLayout->addWidget(avgPowerField, 2, 0, Qt::AlignLeft);
    powerLayout->addWidget(avgPowerValue, 2, 1, Qt::AlignRight);

    // Main Layout
    auto *layout = new QVBoxLayout();
    layout->addLayout(durationLayout);
    layout->addLayout(distanceLayout);
    layout->addItem(new QSpacerItem(0, 20));

    layout->addLayout(hrmLayout);
    layout->addItem(new QSpacerItem(0, 20));

    layout->addLayout(cadenceLayout);
    layout->addItem(new QSpacerItem(0, 20));

    layout->addLayout(speedLayout);
    layout->addItem(new QSpacerItem(0, 20));

    layout->addLayout(powerLayout);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
    adjustSize();
}

auto WorkoutSummaryPanel::handleWorkoutEvent(const WorkoutEvent &data) const -> void {
    spdlog::info("WorkoutSummaryPanel::workoutSummaryReceived");

    durationValue->setText(QString::fromStdString(Data::DURATION.value(data)));
    distanceValue->setText(QString::fromStdString(Data::DISTANCE.value(data)));

    auto is_finished = data.state == WorkoutState::FINISHED;
    maxHrmValue->setText(is_finished ? QString::number(data.hrm.max) : Constants::Values::EMPTY_DATA.data());
    minHrmValue->setText(is_finished ? QString::number(data.hrm.min) : Constants::Values::EMPTY_DATA.data());
    avgHrmValue->setText(is_finished ? QString::number(data.hrm.avg) : Constants::Values::EMPTY_DATA.data());

    maxCadenceValue->setText(is_finished
                                 ? QString::number(data.cadence.max)
                                 : Constants::Values::EMPTY_DATA.data());
    minCadenceValue->setText(is_finished
                                 ? QString::number(data.cadence.min)
                                 : Constants::Values::EMPTY_DATA.data());
    avgCadenceValue->setText(is_finished
                                 ? QString::number(data.cadence.avg)
                                 : Constants::Values::EMPTY_DATA.data());

    maxSpeedValue->setText(is_finished
                               ? QString::number(data.speed.max * 0.01, 'f', 1)
                               : Constants::Values::EMPTY_DATA.data());
    minSpeedValue->setText(is_finished
                               ? QString::number(data.speed.min * 0.01, 'f', 1)
                               : Constants::Values::EMPTY_DATA.data());
    avgSpeedValue->setText(is_finished
                               ? QString::number(data.speed.avg * 0.01, 'f', 1)
                               : Constants::Values::EMPTY_DATA.data());

    maxPowerValue->setText(is_finished ? QString::number(data.power.max) : Constants::Values::EMPTY_DATA.data());
    minPowerValue->setText(is_finished ? QString::number(data.power.min) : Constants::Values::EMPTY_DATA.data());
    avgPowerValue->setText(is_finished ? QString::number(data.power.avg) : Constants::Values::EMPTY_DATA.data());
}
