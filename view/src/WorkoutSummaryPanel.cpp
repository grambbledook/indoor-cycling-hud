#include "WorkoutSummaryPanel.h"

#include <Constants.h>
#include "TrainerWindow.h"

#include <spdlog/spdlog.h>

#include <QGridLayout>
#include <QTableWidget>
#include <QHeaderView>


WorkoutSummaryPanel::WorkoutSummaryPanel(QWidget *parent): QMainWindow(
    parent) {
    const auto tableWidget = new QTableWidget(7, 4, this); // 5 rows, 4 columns

    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);

    auto durationItem = new QTableWidgetItem(Constants::Labels::DURATION.data());
    durationItem->setFlags(durationItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(DURATION_ROW, 0, durationItem);

    duration = new QTableWidgetItem("--");
    duration->setFlags(duration->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(DURATION_ROW, 1, duration);
    tableWidget->setSpan(DURATION_ROW, 1, 1, 3); // Span across 3 columns

    auto maxColumnItem = new QTableWidgetItem(Constants::Labels::MAX.data());
    maxColumnItem->setFlags(maxColumnItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEADER_ROW, 1, maxColumnItem);

    auto minColumnItem = new QTableWidgetItem(Constants::Labels::MIN.data());
    minColumnItem->setFlags(minColumnItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEADER_ROW, 2, minColumnItem);

    auto avgColumnItem = new QTableWidgetItem(Constants::Labels::AVG.data());
    avgColumnItem->setFlags(avgColumnItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEADER_ROW, 3, avgColumnItem);

    auto heartRateItem = new QTableWidgetItem(Constants::Labels::HEART_RATE.data());
    heartRateItem->setFlags(heartRateItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 0, heartRateItem);

    maxHrm = new QTableWidgetItem("--");
    maxHrm->setFlags(maxHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 1, maxHrm);

    minHrm = new QTableWidgetItem("--");
    minHrm->setFlags(minHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 2, minHrm);

    avgHrm = new QTableWidgetItem("--");
    avgHrm->setFlags(avgHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 3, avgHrm);

    auto speedItem = new QTableWidgetItem(Constants::Labels::SPEED.data());
    speedItem->setFlags(speedItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 0, speedItem);

    maxSpeed = new QTableWidgetItem("--");
    maxSpeed->setFlags(maxSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 1, maxSpeed);

    minSpeed = new QTableWidgetItem("--");
    minSpeed->setFlags(minSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 2, minSpeed);

    avgSpeed = new QTableWidgetItem("--");
    avgSpeed->setFlags(avgSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 3, avgSpeed);

    auto cadenceItem = new QTableWidgetItem(Constants::Labels::CADENCE.data());
    cadenceItem->setFlags(cadenceItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 0, cadenceItem);

    maxCadence = new QTableWidgetItem("--");
    maxCadence->setFlags(maxCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 1, maxCadence);

    minCadence = new QTableWidgetItem("--");
    minCadence->setFlags(minCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 2, minCadence);

    avgCadence = new QTableWidgetItem("--");
    avgCadence->setFlags(avgCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 3, avgCadence);

    auto powerItem = new QTableWidgetItem(Constants::Labels::POWER.data());
    powerItem->setFlags(powerItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 0, powerItem);

    maxPower = new QTableWidgetItem("--");
    maxPower->setFlags(maxPower->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 1, maxPower);

    minPower = new QTableWidgetItem("--");
    minPower->setFlags(minPower->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 2, minPower);

    avgPower = new QTableWidgetItem("--");
    avgPower->setFlags(avgPower->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 3, avgPower);

    auto *layout = new QGridLayout(this);
    layout->addWidget(tableWidget, 0, 0, 1, 2, Qt::AlignCenter);
    layout->setColumnStretch(1, 1);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
    adjustSize();
}

void WorkoutSummaryPanel::handleWorkoutSummaryEvent(const WorkoutSummary &data) {
    spdlog::info("WorkoutSummaryPanel::workoutSummaryReceived");

    duration->setText(QString::number(data.duration));

    maxHrm->setText(QString::number(data.hrm.max));
    minHrm->setText(QString::number(data.hrm.min));
    avgHrm->setText(QString::number(data.hrm.avg));

    maxCadence->setText(QString::number(data.cadence.max));
    minCadence->setText(QString::number(data.cadence.min));
    avgCadence->setText(QString::number(data.cadence.avg));

    maxSpeed->setText(QString::number(data.speed.max));
    minSpeed->setText(QString::number(data.speed.min));
    avgSpeed->setText(QString::number(data.speed.avg));

    maxPower->setText(QString::number(data.power.max));
    minPower->setText(QString::number(data.power.min));
    avgPower->setText(QString::number(data.power.avg));
}
