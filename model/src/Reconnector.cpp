//
// Created by denis on 11/1/2024.
//

#include "Reconnector.h"

#include <future>
#include <spdlog/spdlog.h>

using namespace std::chrono;

Reconnector::Reconnector(
    const std::shared_ptr<Model> &model,
    const std::shared_ptr<HrmNotificationService> &hrmService,
    const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscService,
    const std::shared_ptr<PowerNotificationService> &pwrService
): model(model),
   hrmService(hrmService),
   cscService(cscService),
   pwrService(pwrService) {
}

auto Reconnector::registerDevice(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    const auto deviceId = device->deviceId();
    spdlog::info("Received disconnection signal for device_id={}", deviceId);

    if (devices.contains(deviceId)) {
        spdlog::info("  device_id={} already registered, skipping", deviceId);
        return;
    }

    const auto services = model->getDeviceServices(deviceId);
    if (services.empty()) {
        spdlog::info("  No services registered for device_id={}.", deviceId);
        return;
    }

    const auto now = system_clock::now().time_since_epoch();
    devices[deviceId] = device;
    reconnects[deviceId] = duration_cast<milliseconds>(now).count();
}

auto Reconnector::hasSomething() -> bool {
    std::lock_guard guard(mutex);

    return !devices.empty();
}

auto Reconnector::tok() -> void {
    std::lock_guard guard(mutex);

    if (reconnects.empty()) {
        return;
    }

    const auto systemTime = system_clock::now().time_since_epoch();
    const auto now = duration_cast<milliseconds>(systemTime).count();
    for (const auto &[deviceId, timestamp]: reconnects) {
        if (now - timestamp < RECONNECTION_DELAY_MS) {
            spdlog::info("Reconnection for device_id={} has already been recently attempted", deviceId);
            continue;
        }

        auto services = model->getDeviceServices(deviceId);
        if (services.empty()) {
            spdlog::info("  No services registered for device_id={}.", deviceId);

            devices.erase(deviceId);
            reconnects.erase(deviceId);
            continue;
        }

        auto device = devices[deviceId];
        for (const auto &service: services) {
            if (service == Service::HEART_RATE) {
                hrmService->setDevice(device);
            }

            if (service == Service::CADENCE || service == Service::SPEED) {
                cscService->setDevice(device);
            }

            if (service == Service::POWER) {
                pwrService->setDevice(device);
            }
        }

        reconnects[deviceId] = now;
    }
}

auto Reconnector::handleReconnect(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    const auto deviceId = device->deviceId();
    spdlog::info("Received connection signal for device_id={}", deviceId);

    if (!devices.contains(deviceId)) {
        spdlog::info("  device_id={} not registered, skipping", deviceId);
        return;
    }

    devices.erase(deviceId);
    reconnects.erase(deviceId);
}
