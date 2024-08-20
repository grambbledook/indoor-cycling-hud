#pragma once
#include <qtablewidget.h>

#include "AppWindow.h"
#include "Labels.h"

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

    void handleWorkoutEvent(const WorkoutEvent &data);

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

    TextLabel *durationField;
    ValueLabel *durationValue;

    TextLabel *maxHrmField;
    ValueLabel *maxHrmValue;
    TextLabel *minHrmField;
    ValueLabel *minHrmValue;
    TextLabel *avgHrmField;
    ValueLabel *avgHrmValue;

    TextLabel *maxCadenceField;
    ValueLabel *maxCadenceValue;
    TextLabel *minCadenceField;
    ValueLabel *minCadenceValue;
    TextLabel *avgCadenceField;
    ValueLabel *avgCadenceValue;

    TextLabel *maxSpeedField;
    ValueLabel *maxSpeedValue;
    TextLabel *minSpeedField;
    ValueLabel *minSpeedValue;
    TextLabel *avgSpeedField;
    ValueLabel *avgSpeedValue;

    TextLabel *maxPowerField;
    ValueLabel *maxPowerValue;
    TextLabel *minPowerField;
    ValueLabel *minPowerValue;
    TextLabel *avgPowerField;
    ValueLabel *avgPowerValue;
};
