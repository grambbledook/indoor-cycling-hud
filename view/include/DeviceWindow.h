#pragma once
#include "AppWindow.h"
#include "ModelEvents.h"
#include "SelectDevicePanel.h"

class DeviceWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit DeviceWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    auto deviceSelected(const DeviceSelected &event) const -> void;

    auto subscribedToService(const SubscribedToService &event) const -> void;

    auto back() -> void override;

    auto next() -> void override;

private:
    SelectDevicePanel *trainerPanel;
    SelectDevicePanel *heartRateMonitorPanel;
    SelectDevicePanel *cadencePanel;
    SelectDevicePanel *speedPanel;
    SelectDevicePanel *powerPanel;
};
