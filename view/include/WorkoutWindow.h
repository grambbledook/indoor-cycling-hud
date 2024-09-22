#pragma once
#include "AppWindow.h"

#include "MetricsPanel.h"

class WorkoutWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    auto measurementsReceived(const WorkoutEvent &data) const -> void;

    auto next() -> void override;

private:
    MetricsPanel *heartRateMonitorPanel;
    MetricsPanel *cadenceSensorPanel;
    MetricsPanel *powerMeterPanel;
    MetricsPanel *speedSensorPanel;
};
