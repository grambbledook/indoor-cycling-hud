#pragma once
#include "AppWindow.h"

#include "MetricsPanel.h"

class WorkoutWindow final : public AppWindow {
    Q_OBJECT

public:
    void measurementsReceived(const WorkoutEvent &data);

    explicit WorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void next() override;

private:
    MetricsPanel *heartRateMonitorPanel;
    MetricsPanel *cadenceSensorPanel;
    MetricsPanel *powerMeterPanel;
    MetricsPanel *speedSensorPanel;
};
