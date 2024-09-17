#pragma once
#include "AppWindow.h"
#include "ModelEvents.h"
#include "SelectDevicePanel.h"

class SensorsWindow final : public AppWindow {
    Q_OBJECT

public:
    void measurementsReceived(const WorkoutEvent &measurements_update) const;

    explicit SensorsWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void deviceSelected(const DeviceSelected &event) const;

    void back() override;

    void next() override;

private:
    SelectDevicePanel *heartRateMonitorPanel;
    SelectDevicePanel *cadencePanel;
    SelectDevicePanel *speedPanel;
    SelectDevicePanel *powerPanel;
};
