#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

#include "BleDeviceServices.h"
#include "Formula.h"
#include "Model.h"


using std::chrono::duration_cast;
using std::chrono::seconds;

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
    std::lock_guard guard(mutex);

    spdlog::info("Starting new workout  ");
    storage->newWorkout();
    storage->startWorkout();

    publishWorkoutEvent(WorkoutState::STARTED, notifications.summary);
    spdlog::info("  Workput started");
}

void Model::stopWorkout() {
    std::lock_guard guard(mutex);

    spdlog::info("Stopping workout");
    storage->endWorkout();

    publishWorkoutEvent(WorkoutState::FINISHED, notifications.summary);
    spdlog::info("  Workout stopped");
}

void Model::recordHeartData(const MeasurementEvent<HrmMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*hrmState.device != *event.device) {
        return;
    }

    buffer.heartRate = {event.measurement.hrm, system_clock::now()};
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

    const unsigned int cadence = BLE::Math::computeCadence(lcet, prevLcet, ccr, prevCcr);
    spdlog::trace("lcet: {}, prevLcet: {}, ccr: {}, prevCcr: {}, cadence: {}", lcet, prevLcet, ccr, prevCcr, cadence);

    buffer.cadence = {cadence, system_clock::now()};
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

    const unsigned int speed = BLE::Math::computeSpeed(
        lwet, prevLwet, cwr, prevCwr,
        getWheelCircumferenceInMM(wheelSize)
    );

    buffer.speed = {speed, system_clock::now()};
}

void Model::recordPowerData(const MeasurementEvent<PowerMeasurement> &event) {
    std::lock_guard guard(mutex);
    if (*powerState.device != *event.device) {
        return;
    }

    buffer.power = {event.measurement.power, system_clock::now()};
}

void Model::recordTrainerData(const MeasurementEvent<GeneralData> &event) {
    std::lock_guard guard(mutex);
}

void Model::recordTrainerData(const MeasurementEvent<GeneralSettings> &event) {
    std::lock_guard guard(mutex);
}

void Model::recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) {
    std::lock_guard guard(mutex);
}

void Model::tick() {
    std::lock_guard guard(mutex);

    const auto now = system_clock::now();

    constexpr int MAX_RELEVANCE_SECONDS = 1;

    const auto hrm = duration_cast<seconds>(now - buffer.heartRate.timestamp).count() > MAX_RELEVANCE_SECONDS
                         ? 0
                         : buffer.heartRate.value;

    storage->aggregateHeartRate(hrm);

    const auto cadence = duration_cast<seconds>(now - buffer.cadence.timestamp).count() > MAX_RELEVANCE_SECONDS
                             ? 0
                             : buffer.cadence.value;
    storage->aggregateCadence(cadence);

    const auto speed = duration_cast<seconds>(now - buffer.speed.timestamp).count() > MAX_RELEVANCE_SECONDS
                           ? 0
                           : buffer.speed.value;
    storage->aggregateSpeed(speed);

    const auto power = duration_cast<seconds>(now - buffer.power.timestamp).count() > MAX_RELEVANCE_SECONDS
                           ? 0
                           : buffer.power.value;
    storage->aggregatePower(power);

    publishWorkoutEvent(WorkoutState::IN_PROGRESS, notifications.measurements);
}

void Model::publishWorkoutEvent(const WorkoutState status, Channel<WorkoutEvent> &channel) {
    std::lock_guard guard(mutex);

    const auto duration = status == WorkoutState::IN_PROGRESS
                              ? storage->getCurrentWorkoutDuration()
                              : storage->getTotalWorkoutDuration();
    const auto hrm = storage->getHeartRate();
    const auto cadence = storage->getCadence();
    const auto power = storage->getPower();

    auto speed = storage->getSpeed();
    spdlog::trace("Speed: {}, avg Speed: {}, duration: {}", speed.val, speed.avg, duration);
    speed.val *= getSpeedConversionFactor(distanceUnit);
    speed.avg *= getSpeedConversionFactor(distanceUnit);
    speed.windowedAvg *= getSpeedConversionFactor(distanceUnit);
    speed.min *= getSpeedConversionFactor(distanceUnit);
    speed.max *= getSpeedConversionFactor(distanceUnit);

    const auto distance = BLE::Math::computeDistance(speed.val, duration);
    spdlog::trace("   Speed: {}, avg Speed: {}, distance: {}", speed.val, speed.avg, distance);
    const auto summary = WorkoutEvent{
        status, duration, distance, distanceUnit, hrm, cadence, speed, power
    };

    channel.publish(summary);
}
