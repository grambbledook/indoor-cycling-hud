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

    void measurementsReceived(const WorkoutData &data);

private:
    void updateTextLabel();
private:
    Data::DataField dataField;

    TextLabel *label;
    ValueLabel *valueLabel;
};
