#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

#include "BleDeviceServices.h"
#include "Formula.h"
#include "Model.h"

#include "Formula.h"

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
    notifications.deviceDiscovered.publish(DeviceDiscovered{newDevice});
}

std::vector<std::shared_ptr<Device> > Model::getDevices(const GattService *service) {
    std::lock_guard guard(mutex);

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
    std::lock_guard guard(mutex);

    if (device->services.contains(BLE::Services::HRM)) {
        setHeartRateMonitor(device);
    }
    if (device->services.contains(BLE::Services::CSC)) {
        setCadenceSensor(device);
        setSpeedSensor(device);
    }
    if (device->services.contains(BLE::Services::PWR)) {
        setPowerMeter(device);
    }
    if (device->services.contains(BLE::Services::FEC_BIKE_TRAINER)) {
        setBikeTrainer(device);
    }
}

void Model::setHeartRateMonitor(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);
    if (hrmState.device and hrmState.device->deviceId() == device->deviceId()) {
        return;
    }
    hrmState.device = device;
    notifications.deviceSelected.publish(DeviceSelected{Service::HEART_RATE, hrmState.device});
}

void Model::setCadenceSensor(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);
    if (cadenceState.device and cadenceState.device->deviceId() == device->deviceId()) {
        return;
    }
    cadenceState.device = device;
    notifications.deviceSelected.publish(DeviceSelected{Service::CADENCE, cadenceState.device});
}

void Model::setSpeedSensor(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);
    if (speedState.device and speedState.device->deviceId() == device->deviceId()) {
        return;
    }
    speedState.device = device;
    notifications.deviceSelected.publish(DeviceSelected{Service::SPEED, speedState.device});
}

void Model::setPowerMeter(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);
    if (powerState.device and powerState.device->deviceId() == device->deviceId()) {
        return;
    }
    powerState.device = device;
    notifications.deviceSelected.publish(DeviceSelected{Service::POWER, powerState.device});
}

void Model::setBikeTrainer(const std::shared_ptr<Device> &device) {
    std::lock_guard guard(mutex);
    spdlog::info("Dummy: Model::setBikeTrainer: {}", device->deviceId());
}

void Model::setSpeedUnit(const DistanceUnit unit) {
    std::lock_guard guard(mutex);
    this->distanceUnit = unit;
}

void Model::setWheelSize(const WheelSize size) {
    std::lock_guard guard(mutex);
    this->wheelSize = size;
}

void Model::startWorkout() {
    spdlog::info("Starting new workout  ");
    storage->newWorkout();

    auto event = WorkoutEvent{
        true, 0, 0, distanceUnit, Aggregate{}, Aggregate{}, Aggregate{}, Aggregate{}
    };

    notifications.summary.publish(event);
    spdlog::info("  Workput started");
}

void Model::stopWorkout() {
    std::lock_guard guard(mutex);
    spdlog::info("Stopping workout");

    const auto duration = storage->getWorkoutDuration();
    const auto hrm = storage->getHeartRate();
    const auto cadence = storage->getCadence();
    const auto power = storage->getPower();

    auto speed = storage->getSpeed();
    speed.val *= getSpeedConversionFactor(distanceUnit);
    speed.avg *= getSpeedConversionFactor(distanceUnit);
    speed.windowedAvg *= getSpeedConversionFactor(distanceUnit);
    speed.min *= getSpeedConversionFactor(distanceUnit);
    speed.max *= getSpeedConversionFactor(distanceUnit);

    const auto distance = static_cast<long>(duration * speed.avg * getDistanceConversionFactor(distanceUnit));
    const auto summary = WorkoutEvent{
        true, duration, distance, distanceUnit, hrm, cadence, speed, power
    };

    notifications.summary.publish(summary);
    spdlog::info("  Workout stopped");
}

void Model::recordHeartData(const MeasurementEvent<HrmMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*hrmState.device != *event.device) {
        return;
    }

    storage->aggregateHeartRate(event.measurement.hrm);
    publishUpdate();
}

void Model::recordCadenceData(const MeasurementEvent<CadenceMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*cadenceState.device != *event.device) {
        return;
    }

    cadenceState.recordMetric(std::pair{event.measurement.ccr, event.measurement.lcet});

    auto [events, dataPresent] = cadenceState.getLastN(2);
    auto [prevCcr, prevLcet, ccr, lcet] = std::tuple_cat(events[0], events[1]);

    if (lcet == prevLcet and dataPresent) {
        cadenceState.unrecordMetric();
        return;
    }

    const auto cadence = BLE::Math::computeCadence(lcet, prevLcet, ccr, prevCcr);
    spdlog::trace("lcet: {}, prevLcet: {}, ccr: {}, prevCcr: {}, cadence: {}", lcet, prevLcet, ccr, prevCcr, cadence);

    storage->aggregateCadence(cadence);
    publishUpdate();
}

void Model::recordSpeedData(const MeasurementEvent<SpeedMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*speedState.device != *event.device) {
        return;
    }

    speedState.recordMetric(std::pair{event.measurement.cwr, event.measurement.lwet});

    auto [events, dataPresent] = speedState.getLastN(2);
    auto [prevCwr, prevLwet, cwr, lwet] = std::tuple_cat(events[0], events[1]);

    if (lwet == prevLwet and dataPresent) {
        speedState.unrecordMetric();
        return;
    }

    const auto speed = BLE::Math::computeSpeed(
        lwet, prevLwet, cwr, prevCwr,
        getWheelCircumferenceInMM(wheelSize)
    );
    spdlog::trace("lwet: {}, prevLwet: {}, cwr: {}, prevCwr: {}, speed: {}", lwet, prevLwet, cwr, prevCwr, speed);
    storage->aggregateSpeed(speed);
    publishUpdate();
}

void Model::recordPowerData(const MeasurementEvent<PowerMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*powerState.device != *event.device) {
        return;
    }

    storage->aggregatePower(event.measurement.power);
    publishUpdate();
}

void Model::recordTrainerData(const MeasurementEvent<GeneralData> &event) {
    std::lock_guard guard(mutex);
}

void Model::recordTrainerData(const MeasurementEvent<GeneralSettings> &event) {
    std::lock_guard guard(mutex);
}

void Model::recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) {
    std::lock_guard guard(mutex);
    publishUpdate();
}

void Model::publishUpdate() {
    std::lock_guard guard(mutex);

    const auto duration = storage->getWorkoutDuration();
    const auto hrm = storage->getHeartRate();
    const auto cadence = storage->getCadence();
    const auto power = storage->getPower();

    auto speed = storage->getSpeed();
    speed.val *= getSpeedConversionFactor(distanceUnit);
    speed.avg *= getSpeedConversionFactor(distanceUnit);
    speed.windowedAvg *= getSpeedConversionFactor(distanceUnit);
    speed.min *= getSpeedConversionFactor(distanceUnit);
    speed.max *= getSpeedConversionFactor(distanceUnit);

    const auto distance = BLE::Math::computeDistance(speed.avg, duration, distanceUnit);
    const auto aggregate = WorkoutData{
        duration, distance, distanceUnit, hrm, cadence, speed, power
    };

    notifications.measurements.publish(aggregate);
}
