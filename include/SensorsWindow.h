#pragma once
#include "AppWindow.h"
#include "SelectDevicePanel.h"

class SensorsWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit SensorsWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void deviceSelected(const std::shared_ptr<Device> &device) override;

    void back() override;

    void next() override;

private:
    SelectDevicePanel *heartRateMonitorPanel;
    SelectDevicePanel *cadencePanel;
    SelectDevicePanel *speedPanel;
    SelectDevicePanel *powerPanel;
};
