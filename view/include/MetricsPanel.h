#pragma once
#include <QWidget>

#include "DataFields.h"

class MetricsPanel final : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(const Data::DataField &dataField, QWidget *parent = nullptr);

public slots:
    auto next() -> void;
    auto prev() -> void;

    auto measurementsReceived(const WorkoutEvent &data) -> void;

private:
    auto updateTextLabel() const -> void;

    auto updateValueLabel() const -> void;

    TextLabel *label;
    ValueLabel *valueLabel;

    Data::DataField dataField;
    std::shared_ptr<WorkoutEvent> data;
};
