#include "WorkoutSummaryPanel.h"

#include <Constants.h>
#include "TrainerWindow.h"

#include <spdlog/spdlog.h>

#include <QGridLayout>
#include <QTableWidget>
#include <QHeaderView>

#include "DataFields.h"


WorkoutSummaryPanel::WorkoutSummaryPanel(QWidget *parent): QMainWindow(
    parent) {
    const auto tableWidget = new QTableWidget(7, 4, this); // 5 rows, 4 columns

    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);

    auto durationItem = new QTableWidgetItem(Constants::Labels::DURATION.data());
    durationItem->setFlags(durationItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(DURATION_ROW, 0, durationItem);


    duration = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
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

    maxHrm = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    maxHrm->setFlags(maxHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 1, maxHrm);

    minHrm = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    minHrm->setFlags(minHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 2, minHrm);

    avgHrm = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    avgHrm->setFlags(avgHrm->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(HEAR_RATE_ROW, 3, avgHrm);

    auto speedItem = new QTableWidgetItem(Constants::Labels::SPEED.data());
    speedItem->setFlags(speedItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 0, speedItem);

    maxSpeed = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    maxSpeed->setFlags(maxSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 1, maxSpeed);

    minSpeed = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    minSpeed->setFlags(minSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 2, minSpeed);

    avgSpeed = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    avgSpeed->setFlags(avgSpeed->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(SPEED_ROW, 3, avgSpeed);

    auto cadenceItem = new QTableWidgetItem(Constants::Labels::CADENCE.data());
    cadenceItem->setFlags(cadenceItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 0, cadenceItem);

    maxCadence = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    maxCadence->setFlags(maxCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 1, maxCadence);

    minCadence = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    minCadence->setFlags(minCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 2, minCadence);

    avgCadence = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    avgCadence->setFlags(avgCadence->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(CADENCE_ROW, 3, avgCadence);

    auto powerItem = new QTableWidgetItem(Constants::Labels::POWER.data());
    powerItem->setFlags(powerItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 0, powerItem);

    maxPower = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    maxPower->setFlags(maxPower->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 1, maxPower);

    minPower = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
    minPower->setFlags(minPower->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(POWER_ROW, 2, minPower);

    avgPower = new QTableWidgetItem(Constants::Values::EMPTY_DATA.data());
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

void WorkoutSummaryPanel::handleWorkoutEvent(const WorkoutEvent &data) {
    spdlog::info("WorkoutSummaryPanel::workoutSummaryReceived");

    duration->setText(QString::fromStdString(Data::DURATION.value(data)));

    maxHrm->setText(data.isFinished ? QString::number(data.hrm.max) : Constants::Values::EMPTY_DATA.data());
    minHrm->setText(data.isFinished ? QString::number(data.hrm.min) : Constants::Values::EMPTY_DATA.data());
    avgHrm->setText(data.isFinished ? QString::number(data.hrm.avg) : Constants::Values::EMPTY_DATA.data());

    maxCadence->setText(data.isFinished ? QString::number(data.cadence.max) : Constants::Values::EMPTY_DATA.data());
    minCadence->setText(data.isFinished ? QString::number(data.cadence.min) : Constants::Values::EMPTY_DATA.data());
    avgCadence->setText(data.isFinished ? QString::number(data.cadence.avg) : Constants::Values::EMPTY_DATA.data());

    maxSpeed->setText(data.isFinished ? QString::number(data.speed.max) : Constants::Values::EMPTY_DATA.data());
    minSpeed->setText(data.isFinished ? QString::number(data.speed.min) : Constants::Values::EMPTY_DATA.data());
    avgSpeed->setText(data.isFinished ? QString::number(data.speed.avg) : Constants::Values::EMPTY_DATA.data());

    maxPower->setText(data.isFinished ? QString::number(data.power.max) : Constants::Values::EMPTY_DATA.data());
    minPower->setText(data.isFinished ? QString::number(data.power.min) : Constants::Values::EMPTY_DATA.data());
    avgPower->setText(data.isFinished ? QString::number(data.power.avg) : Constants::Values::EMPTY_DATA.data());
}
