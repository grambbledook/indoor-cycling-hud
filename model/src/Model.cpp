#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

#include "BleDeviceServices.h"
#include "Formula.h"
#include "Model.h"


using std::chrono::duration_cast;
using std::chrono::seconds;

auto Model::addDevice(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    const auto oldDevice = devices[device->deviceId()];
    if (oldDevice && *oldDevice == *device) {
        return;
    }

    const auto newDevice = oldDevice ? std::make_shared<Device>(*oldDevice | *device) : device;

    if (oldDevice && *newDevice == *oldDevice) {
        return;
    }
    devices[device->deviceId()] = newDevice;
    notifications.deviceDiscovered.publish(DeviceDiscovered{newDevice});
}

auto Model::getDevices(const GattService *service) -> std::vector<std::shared_ptr<Device> > {
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

auto Model::setDevice(const std::shared_ptr<Device> &device) -> void {
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

auto Model::setHeartRateMonitor(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    if (hrmState.device && hrmState.device->deviceId() == device->deviceId()) {
        return;
    }
    hrmState = {device};
    notifications.deviceSelected.publish(DeviceSelected{Service::HEART_RATE, hrmState.device});
}

auto Model::setCadenceSensor(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    if (cadenceState.device && cadenceState.device->deviceId() == device->deviceId()) {
        return;
    }
    cadenceState = {device};
    notifications.deviceSelected.publish(DeviceSelected{Service::CADENCE, cadenceState.device});
}

auto Model::setSpeedSensor(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    if (speedState.device && speedState.device->deviceId() == device->deviceId()) {
        return;
    }
    speedState = {device};
    notifications.deviceSelected.publish(DeviceSelected{Service::SPEED, speedState.device});
}

auto Model::setPowerMeter(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    if (powerState.device && powerState.device->deviceId() == device->deviceId()) {
        return;
    }
    powerState = {device};
    notifications.deviceSelected.publish(DeviceSelected{Service::POWER, powerState.device});
}

auto Model::setBikeTrainer(const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    spdlog::info("Dummy: Model::setBikeTrainer: {}", device->deviceId());
}

auto Model::setSpeedUnit(const DistanceUnit unit) -> void {
    std::lock_guard guard(mutex);

    this->distanceUnit = unit;
}

auto Model::setWheelSize(const WheelSize size) -> void {
    std::lock_guard guard(mutex);

    this->wheelSize = size;
}

auto Model::startWorkout() -> void {
    std::lock_guard guard(mutex);

    spdlog::info("Starting new workout  ");
    storage->newWorkout();
    storage->startWorkout();

    publishWorkoutEvent(WorkoutState::STARTED, notifications.summary);
    spdlog::info("  Workput started");
}

auto Model::stopWorkout() -> void {
    std::lock_guard guard(mutex);

    spdlog::info("Stopping workout");
    storage->endWorkout();

    publishWorkoutEvent(WorkoutState::FINISHED, notifications.summary);
    spdlog::info("  Workout stopped");
}

auto Model::recordHeartData(const MeasurementEvent<HrmMeasurement> &event) -> void {
    std::lock_guard guard(mutex);
    if (*hrmState.device != *event.device) {
        return;
    }

    buffer.heartRate = {event.measurement.hrm, system_clock::now()};
}

auto Model::recordCadenceData(const MeasurementEvent<CadenceMeasurement> &event) -> void {
    std::lock_guard guard(mutex);
    if (*cadenceState.device != *event.device) {
        return;
    }

    cadenceState.recordMetric(std::pair{event.measurement.ccr, event.measurement.lcet});

    const auto events = cadenceState.getLastN(2);
    if (!events.has_value()) {
        return;
    }
    const auto &data = events.value();
    auto [prevCcr, prevLcet, ccr, lcet] = std::tuple_cat(data[0], data[1]);
    if (lcet == prevLcet) {
        cadenceState.unrecordMetric();
        return;
    }

    const auto cadence = BLE::Math::computeCadence(lcet, prevLcet, ccr, prevCcr);
    buffer.cadence = {cadence, system_clock::now()};
}

auto Model::recordSpeedData(const MeasurementEvent<SpeedMeasurement> &event) -> void {
    std::lock_guard guard(mutex);
    if (*speedState.device != *event.device) {
        return;
    }

    speedState.recordMetric(std::pair{event.measurement.cwr, event.measurement.lwet});

    const auto events = speedState.getLastN(2);
    if (!events.has_value()) {
        return;
    }

    const auto &data = events.value();
    auto [prevCwr, prevLwet, cwr, lwet] = std::tuple_cat(data[0], data[1]);

    if (lwet == prevLwet) {
        speedState.unrecordMetric();
        return;
    }

    const auto speed = BLE::Math::computeSpeed(lwet, prevLwet, cwr, prevCwr);
    buffer.speed = {speed, system_clock::now()};
}

auto Model::recordPowerData(const MeasurementEvent<PowerMeasurement> &event) -> void {
    std::lock_guard guard(mutex);
    if (*powerState.device != *event.device) {
        return;
    }

    buffer.power = {event.measurement.power, system_clock::now()};
}

auto Model::recordTrainerData(const MeasurementEvent<GeneralData> &event) -> void {
    std::lock_guard guard(mutex);
}

auto Model::recordTrainerData(const MeasurementEvent<GeneralSettings> &event) -> void {
    std::lock_guard guard(mutex);
}

auto Model::recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) -> void {
    std::lock_guard guard(mutex);
}

auto Model::tick() -> void {
    std::lock_guard guard(mutex);

    const auto now = system_clock::now();

    constexpr int MAX_RELEVANCE_SECONDS = 1;

    const auto hrm = duration_cast<seconds>(now - buffer.heartRate.timestamp).count() > MAX_RELEVANCE_SECONDS
                         ? 0
                         : buffer.heartRate.value;

    const auto [cadence, totalCrankRevs] = duration_cast<seconds>(now - buffer.cadence.timestamp).count() >
                                           MAX_RELEVANCE_SECONDS
                                               ? std::make_tuple(0L, 0UL)
                                               : buffer.cadence.value;

    const auto [speed, totalWheelRevs] = duration_cast<seconds>(now - buffer.speed.timestamp).count() >
                                         MAX_RELEVANCE_SECONDS
                                             ? std::make_tuple(0L, 0UL)
                                             : buffer.speed.value;

    const auto power = duration_cast<seconds>(now - buffer.power.timestamp).count() > MAX_RELEVANCE_SECONDS
                           ? 0
                           : buffer.power.value;

    storage->aggregateHeartRate(hrm);
    storage->aggregateCadence(cadence);
    storage->aggregateSpeed(speed);
    storage->aggregatePower(power);

    storage->aggregate(now.time_since_epoch(), hrm, power, cadence, totalCrankRevs, speed, totalWheelRevs);

    publishWorkoutEvent(WorkoutState::IN_PROGRESS, notifications.measurements);
}

auto Model::publishWorkoutEvent(const WorkoutState status, Channel<WorkoutEvent> &channel) -> void {
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
