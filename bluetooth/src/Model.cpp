#include <algorithm>
#include <iostream>

#include "Events.h"
#include "Model.h"

#include <memory>
#include <unordered_set>

constexpr auto MM_TO_KM = 1.0 / 1000000;
constexpr auto MS_TO_MIN = 1.0 / (60 * 1024);
constexpr auto MS_TO_HOUR = 1.0 / (60 * 60 * 1024);
constexpr auto DEFAULT_TIRE_CIRCUMFERENCE_MM = 2168;

std::vector<GattService> operator|(const std::vector<GattService> &lhs, const std::vector<GattService> &rhs) {
    std::vector<GattService> unionResult = lhs;

    std::unordered_set<UUID> existingUUIDs;
    for (const auto &service: lhs) {
        existingUUIDs.insert(service.service_uuid);
    }

    for (const auto &service: rhs) {
        if (!existingUUIDs.contains(service.service_uuid)) {
            unionResult.push_back(service);
            existingUUIDs.insert(service.service_uuid);
        }
    }

    return unionResult;
}

std::shared_ptr<Device> operator|(const std::shared_ptr<Device> &lhs, const std::shared_ptr<Device> &rhs) {
    return std::make_shared<Device>(
        lhs->name,
        lhs->address,
        lhs->services | rhs->services
    );
}

void Model::setHeartRateMonitor(const std::shared_ptr<Device> &device) {
    std::cout << "Model::setHeartRateMonitor: " << device->deviceId() << std::endl;

    const bool sameDevice = hrmState.device->deviceId() == device->deviceId();
    hrmState.device = sameDevice ? hrmState.device | device : device;

    hrmNotifications.deviceSelected.publish(hrmState.device);
}

void Model::setCadenceSensor(const std::shared_ptr<Device> &device) {
    std::cout << "Model::setCadenceSensor: " << device->deviceId() << std::endl;
    const bool sameDevice = cadenceState.device->deviceId() == device->deviceId();
    cadenceState.device = sameDevice ? cadenceState.device | device : device;

    cadenceNotifications.deviceSelected.publish(cadenceState.device);
}

void Model::setSpeedSensor(const std::shared_ptr<Device> &device) {
    std::cout << "Model::setSpeedSensor: " << device->deviceId() << std::endl;
    const bool sameDevice = speedState.device->deviceId() == device->deviceId();
    speedState.device = sameDevice ? speedState.device | device : device;

    speedNotifications.deviceSelected.publish(speedState.device);
}

void Model::setPowerMeter(const std::shared_ptr<Device> &device) {
    std::cout << "Model::setPowerMeter: " << device->deviceId() << std::endl;
    const bool sameDevice = powerState.device->deviceId() == device->deviceId();
    powerState.device = sameDevice ? powerState.device | device : device;

    powerNotifications.deviceSelected.publish(powerState.device);
}

void Model::setBikeTrainer(const std::shared_ptr<Device> &device) {
    std::cout << "Model::setBikeTrainer: " << device->deviceId() << std::endl;
}

void Model::recordHeartData(const MeasurementEvent<HrmMeasurement> &event) {
    if (*hrmState.device != *event.device) {
        std::cout << "Device mismatch." << std::endl;
        std::cout << "  Should be: " << hrmState.device->deviceId() << " but was: " << event.device->deviceId() <<
                std::endl;
        return;
    }

    hrmState.recordMetric(event.measurement.hrm);
    hrmState.aggregateMetric(event.measurement.hrm);
    std::cout << "  HRM: " << hrmState.stats.latest << " AVG: " << hrmState.stats.average << std::endl;

    hrmNotifications.data.publish(hrmState.stats);
}

void Model::recordCadenceData(const MeasurementEvent<CadenceMeasurement> &event) {
    std::cout << "Model::recordCadenceData" << std::endl;
    if (*cadenceState.device != *event.device) {
        return;
    }

    cadenceState.recordMetric(std::pair{event.measurement.ccr, event.measurement.lcet});

    auto [events, dataPresent] = cadenceState.getLastN(2);
    auto [prevCcr, prevLcet, ccr, lcet] = std::tuple_cat(events[0], events[1]);
    std::cout << "  Prev[" << prevCcr << ", " << prevLcet << "], Cur[" << ccr << ", " << lcet << "]" << std::endl;

    if (lcet == prevLcet and dataPresent) {
        std::cout << "  Same timestamp. Skipping..." << std::endl;
        cadenceState.unrecordMetric();
        return;
    }

    const auto lcetResetCorrection = lcet > prevLcet ? 0 : 0x10000;

    const auto totalRevolutions = ccr - prevCcr;
    const auto timeDelta = lcet - prevLcet + lcetResetCorrection;

    const auto cadence = totalRevolutions / (timeDelta * MS_TO_MIN);

    cadenceState.aggregateMetric(std::round(cadence));
    std::cout << "  CADENCE: " << cadenceState.stats.latest << " AVG: " << cadenceState.stats.average << std::endl;

    cadenceNotifications.data.publish(cadenceState.stats);
}

void Model::recordSpeedData(const MeasurementEvent<SpeedMeasurement> &event) {
    std::cout << "Model::recordSpeedData" << std::endl;
    if (*speedState.device != *event.device) {
        std::cout << "  Device mismatch." << std::endl;
        std::cout << "  Should be: " << speedState.device->deviceId() << " but was: " << event.device->deviceId() <<
                std::endl;
        return;
    }

    speedState.recordMetric(std::pair{event.measurement.cwr, event.measurement.lwet});

    auto [events, dataPresent] = speedState.getLastN(2);
    auto [prevCwr, prevLwet, cwr, lwet] = std::tuple_cat(events[0], events[1]);

    std::cout << "  Prev[" << prevCwr << ", " << prevLwet << "], Cur[" << cwr << ", " << lwet << "]" << std::endl;
    if (lwet == prevLwet and dataPresent) {
        speedState.unrecordMetric();
        return;
    }

    const auto lwetResetCorrection = lwet > prevLwet ? 0 : 0x10000;

    const auto totalRevolutions = cwr - prevCwr;
    const auto timeDelta = lwet - prevLwet + lwetResetCorrection;

    const auto totalKmh = totalRevolutions * DEFAULT_TIRE_CIRCUMFERENCE_MM * MM_TO_KM;
    const auto speed = totalKmh / timeDelta * MS_TO_HOUR;

    speedState.aggregateMetric(std::round(speed));
    std::cout << "  SPEED: " << speedState.stats.latest << " AVG: " << speedState.stats.average << std::endl;

    speedNotifications.data.publish(speedState.stats);
}

void Model::recordPowerData(const MeasurementEvent<PowerMeasurement> &event) {
    if (*powerState.device != *event.device) {
        return;
    }

    powerState.recordMetric(event.measurement.power);
    powerState.aggregateMetric(event.measurement.power);
    std::cout << "  POWER: " << powerState.stats.latest << " AVG: " << powerState.stats.average << std::endl;

    powerNotifications.data.publish(powerState.stats);
}

void Model::recordTrainerData(const MeasurementEvent<GeneralData> &event) {
}

void Model::recordTrainerData(const MeasurementEvent<GeneralSettings> &event) {
    std::cout << "  General FE Settings: " << event.measurement.cycleLength << " " << event.measurement.incline << " "
            << event.measurement.resistance << " " << event.measurement.feState.state << std::endl;
}

void Model::recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) {
    std::cout << "  Specific Trainer Data: " << event.measurement.targetPowerLimits << std::endl;
}
