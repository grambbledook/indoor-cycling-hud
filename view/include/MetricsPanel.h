#pragma once
#include <QWidget>

#include "DataFields.h"

class MetricsPanel final : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(Data::DataField dataField, QWidget *parent = nullptr);

public slots:
    void next();
    void prev();

    void measurementsReceived(const WorkoutEvent &data);

private:
    void updateTextLabel();

    void updateValueLabel();

private:
    TextLabel *label;
    ValueLabel *valueLabel;

    Data::DataField dataField;
    std::shared_ptr<WorkoutEvent> data;
};
