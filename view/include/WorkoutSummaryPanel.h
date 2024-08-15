#pragma once
#include <qtablewidget.h>

#include "AppWindow.h"

constexpr auto DURATION_ROW = 0;
constexpr auto HEADER_ROW = 2;
constexpr auto HEAR_RATE_ROW = 3;
constexpr auto CADENCE_ROW = 4;
constexpr auto SPEED_ROW = 5;
constexpr auto POWER_ROW = 6;

class WorkoutSummaryPanel final : public QMainWindow {
    Q_OBJECT

public:
    explicit WorkoutSummaryPanel(QWidget *parent = nullptr);

    void handleWorkoutSummaryEvent(const WorkoutSummary &data);

private:
    QTableWidgetItem *duration;

    QTableWidgetItem *maxHrm;
    QTableWidgetItem *minHrm;
    QTableWidgetItem *avgHrm;

    QTableWidgetItem *maxCadence;
    QTableWidgetItem *minCadence;
    QTableWidgetItem *avgCadence;

    QTableWidgetItem *maxSpeed;
    QTableWidgetItem *minSpeed;
    QTableWidgetItem *avgSpeed;

    QTableWidgetItem *maxPower;
    QTableWidgetItem *minPower;
    QTableWidgetItem *avgPower;
};
