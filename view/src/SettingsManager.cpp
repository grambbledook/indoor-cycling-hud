#include "SettingsManager.h"

#include <spdlog/spdlog.h>

#include "WorkoutSettings.h"

auto SettingsManager::initialise() const -> void {
    spdlog::info("Loading workout settings...");

    const auto [services, workout] = loadWorkoutSettings();

    auto devices = std::unordered_map<std::string, std::shared_ptr<Device> >();
    if (services.hrm) {
        spdlog::info("  Connecting to HRM device: {}", services.hrm.value());
        const auto hrm = fromDeviceId(services.hrm.value());
        model->addDevice(hrm);
        model->setDevice(Service::HEART_RATE, hrm);

        devices[hrm->deviceId()] = hrm;
    }

    if (services.power) {
        spdlog::info("  Connecting to Power device: {}", services.power.value());
        const auto pwr = fromDeviceId(services.power.value());
        model->addDevice(pwr);
        model->setDevice(Service::POWER, pwr);

        devices[pwr->deviceId()] = pwr;
    }

    if (services.cadence) {
        spdlog::info("  Connecting to Cadence device: {}", services.cadence.value());
        const auto cad = fromDeviceId(services.cadence.value());
        model->addDevice(cad);
        model->setDevice(Service::CADENCE, cad);

        devices[cad->deviceId()] = cad;
    }

    if (services.speed) {
        spdlog::info("  Connecting to Speed device: {}", services.speed.value());
        const auto spd = fromDeviceId(services.speed.value());
        model->addDevice(spd);
        model->setDevice(Service::SPEED, spd);

        devices[spd->deviceId()] = spd;
    }

    for (const auto &[deviceId, device]: devices) {
        eventbus->publish(DeviceConnectionEvent(device, DISCONNECTED));
    }
}

auto SettingsManager::store() const -> void {
    WorkoutSettings settings;

    if (const auto hrm = model->getHrmDevice()) {
        settings.services.hrm = hrm->deviceId();
    }

    if (const auto power = model->getPowerDevice()) {
        settings.services.power = power->deviceId();
    }

    if (const auto cadence = model->getCadenceDevice()) {
        settings.services.cadence = cadence->deviceId();
    }

    if (const auto speed = model->getSpeedDevice()) {
        settings.services.speed = speed->deviceId();
    }

    spdlog::info("Storing workout settings...");
    saveWorkoutSettings(settings);
    spdlog::info("  Workout settings saved successfully.");
}
