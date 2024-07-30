#include <algorithm>
#include <iostream>

#include "Events.h"
#include "Model.h"

#include <memory>

#include "Formula.h"
#include "Service.h"


void Model::addDevice(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);

    const auto oldDevice = devices[device->deviceId()];
    if (oldDevice and *oldDevice == *device) {
        return;
    }

    const auto newDevice = oldDevice ? std::make_shared<Device>(*oldDevice | *device) : device;

    if (oldDevice and *newDevice == *oldDevice) {
        return;
    }
    devices[device->deviceId()] = newDevice;
    notifications.deviceDiscovered.publish(newDevice);
}

std::vector<std::shared_ptr<Device> > Model::getDevices(const GattService *service = nullptr) {
    if (service == nullptr) {
        return devices
               | std::views::transform([](const auto &pair) { return pair.second; })
               | std::ranges::to<std::vector<std::shared_ptr<Device> > >();
    }

    return devices
           | std::views::filter([service](const auto &pair) { return pair.second->services.contains(*service); })
           | std::views::transform([](const auto &pair) { return pair.second; })
           | std::ranges::to<std::vector<std::shared_ptr<Device> > >();
}

void Model::setDevice(const std::shared_ptr<Device> &device) {
    if (device->services.contains(Services::HRM)) {
        setHeartRateMonitor(device);
    }
    if (device->services.contains(Services::CSC)) {
        setCadenceSensor(device);
        setSpeedSensor(device);
    }
    if (device->services.contains(Services::PWR)) {
        setPowerMeter(device);
    }
    if (device->services.contains(Services::FEC_BIKE_TRAINER)) {
        setBikeTrainer(device);
    }
}

void Model::setHeartRateMonitor(const std::shared_ptr<Device> &device) {
    if (hrmState.device and hrmState.device->deviceId() == device->deviceId()) {
        std::cout << "Model::setHeartRateMonitor: " << "alredy set" << std::endl;
        return;
    }
    std::cout << "Model::setHeartRateMonitor: " << device->deviceId() << std::endl;
    hrmState.device = device;
    notifications.deviceSelected.publish(hrmState.device);
}

void Model::setCadenceSensor(const std::shared_ptr<Device> &device) {
    if (cadenceState.device and cadenceState.device->deviceId() == device->deviceId()) {
        return;
    }

    cadenceState.device = device;
    notifications.deviceSelected.publish(cadenceState.device);
}

void Model::setSpeedSensor(const std::shared_ptr<Device> &device) {
    if (speedState.device and speedState.device->deviceId() == device->deviceId()) {
        return;
    }

    speedState.device = device;
    publishUpdate();
}

void Model::setPowerMeter(const std::shared_ptr<Device> &device) {
    if (powerState.device and powerState.device->deviceId() == device->deviceId()) {
        return;
    }

    powerState.device = device;
    notifications.deviceSelected.publish(powerState.device);
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

    publishUpdate();
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

    const auto cadence = totalRevolutions * BLE::Math::MS_IN_MIN / timeDelta;

    cadenceState.aggregateMetric(cadence);
    std::cout << "  CADENCE: " << cadenceState.stats.latest << " AVG: " << cadenceState.stats.average << std::endl;

    publishUpdate();
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

    const auto totalKmh = totalRevolutions * BLE::Wheels::DEFAULT_TIRE_CIRCUMFERENCE_MM / BLE::Math::MM_IN_KM;
    const auto speed = totalKmh * BLE::Math::MS_IN_HOUR / timeDelta;

    speedState.aggregateMetric(speed);
    std::cout << "  SPEED: " << speedState.stats.latest << " AVG: " << speedState.stats.average << std::endl;

    publishUpdate();
}

void Model::recordPowerData(const MeasurementEvent<PowerMeasurement> &event) {
    if (*powerState.device != *event.device) {
        return;
    }

    powerState.recordMetric(event.measurement.power);
    powerState.aggregateMetric(event.measurement.power);
    std::cout << "  POWER: " << powerState.stats.latest << " AVG: " << powerState.stats.average << std::endl;

    publishUpdate();
}

void Model::recordTrainerData(const MeasurementEvent<GeneralData> &event) {
}

void Model::recordTrainerData(const MeasurementEvent<GeneralSettings> &event) {
}

void Model::recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) {
    publishUpdate();
}

void Model::publishUpdate() {
    const auto aggregate = MeasurementsUpdate{
        CroppedStatistics{hrmState.stats.latest, hrmState.stats.average},
        CroppedStatistics{cadenceState.stats.latest, cadenceState.stats.average},
        CroppedStatistics{speedState.stats.latest, speedState.stats.average},
        CroppedStatistics{powerState.stats.latest, powerState.stats.average},
    };

    notifications.newMeasurements.publish(aggregate);
}
