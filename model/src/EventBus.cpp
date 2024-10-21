#include "EventBus.h"

#include <spdlog/spdlog.h>
#include "ModelEvents.h"

auto EventBus::publish(const Event &event) -> void {
    if (auto bleConnectionEvent = dynamic_cast<const DeviceConnectionEvent *>(&event)) {
        auto status = bleConnectionEvent->status == ConnectionStatus::CONNECTED ? "CONNECTED" : "DISCONNECTED";

        spdlog::info(
            "BUS: event received. device_id={}, status={}",
            bleConnectionEvent->device->deviceId(), status
        );
    }
}
