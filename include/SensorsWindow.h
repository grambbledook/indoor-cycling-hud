#pragma once
#include "AppWindow.h"
#include "SelectDevicePanel.h"
#include "Stats.h"

class SensorsWindow final : public AppWindow {
    Q_OBJECT

public:
    void measurementsReceived(const MeasurementsUpdate &measurements_update) const;

    explicit SensorsWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void deviceSelected(const std::shared_ptr<Device> &device) const;

    void back() override;

    void next() override;

private:
    SelectDevicePanel *heartRateMonitorPanel;
    SelectDevicePanel *cadencePanel;
    SelectDevicePanel *speedPanel;
    SelectDevicePanel *powerPanel;
};
