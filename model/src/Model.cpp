#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

#include "BleDeviceServices.h"
#include "Formula.h"
#include "Model.h"


using std::chrono::duration_cast;
using std::chrono::seconds;

//
// Device discovery
//

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
    eventBus->publish(DeviceDiscovered(newDevice));
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

//
// Device management
//

auto Model::setDevice(const Service &service, const std::shared_ptr<Device> &device) -> void {
    std::lock_guard guard(mutex);

    if (service == Service::HEART_RATE) {
        if (hrmState.device && hrmState.device->deviceId() == device->deviceId()) {
            return;
        }

        hrmState = {device};
        eventBus->publish(DeviceSelected{Service::HEART_RATE, hrmState.device});
    }

    if (service == Service::CADENCE) {
        if (cadenceState.device && cadenceState.device->deviceId() == device->deviceId()) {
            return;
        }
        cadenceState = {device};
        eventBus->publish(DeviceSelected{Service::CADENCE, cadenceState.device});
    }

    if (service == Service::SPEED) {
        if (speedState.device && speedState.device->deviceId() == device->deviceId()) {
            return;
        }
        speedState = {device};
        eventBus->publish(DeviceSelected{Service::SPEED, speedState.device});
    }

    if (service == Service::POWER) {
        if (powerState.device && powerState.device->deviceId() == device->deviceId()) {
            return;
        }
        powerState = {device};
        eventBus->publish(DeviceSelected{Service::POWER, powerState.device});
    }

    if (service == Service::BIKE_TRAINER) {
        spdlog::warn("Bike trainer not supported");
    }
}

auto Model::setSpeedUnit(const DistanceUnit unit) -> void {
    std::lock_guard guard(mutex);

    this->distanceUnit = unit;
}

auto Model::setWheelSize(const WheelSize size) -> void {
    std::lock_guard guard(mutex);

    this->wheelSize = size;
}

//
// Data recording
//

auto Model::recordData(const MeasurementEvent &event) -> void {
    std::lock_guard guard(mutex);
    spdlog::trace("Recording data for device: {}", event.device->deviceId());

    if (const auto hrm = dynamic_cast<const HrmMeasurement *>(event.measurement.get())) {
        if (*hrmState.device != *event.device) {
            return;
        }

        recordHeartData(*hrm);
    }

    if (const auto cad = dynamic_cast<const CadenceMeasurement *>(event.measurement.get())) {
        if (*cadenceState.device != *event.device) {
            return;
        }

        recordCadenceData(*cad);
    }

    if (const auto spd = dynamic_cast<const SpeedMeasurement *>(event.measurement.get())) {
        if (*cadenceState.device != *event.device) {
            return;
        }

        recordSpeedData(*spd);
    }
}

auto Model::recordHeartData(const HrmMeasurement &event) -> void {
    buffer.heartRate = {event.hrm, system_clock::now()};

    if (!hrmState.active) {
        hrmState.active = true;
        eventBus->publish(SubscribedToService{Service::HEART_RATE, hrmState.device});
    }
}

auto Model::recordPowerData(const PowerMeasurement &event) -> void {
    buffer.power = {event.power, system_clock::now()};

    if (!powerState.active) {
        powerState.active = true;
        eventBus->publish(SubscribedToService{Service::POWER, powerState.device});
    }
}

auto Model::recordCadenceData(const CadenceMeasurement &event) -> void {
    cadenceState.recordMetric(std::pair{event.ccr, event.lcet});

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

    if (!cadenceState.active) {
        cadenceState.active = true;
        eventBus->publish(SubscribedToService{Service::CADENCE, cadenceState.device});
    }
}

auto Model::recordSpeedData(const SpeedMeasurement &event) -> void {
    speedState.recordMetric(std::pair{event.cwr, event.lwet});

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

    if (!speedState.active) {
        speedState.active = true;
        eventBus->publish(SubscribedToService{Service::SPEED, speedState.device});
    }
}

//
// Workout management
//

auto Model::startWorkout() -> void {
    std::lock_guard guard(mutex);

    spdlog::info("Starting new workout  ");
    storage->newWorkout();
    storage->startWorkout();

    publishWorkoutEvent(WorkoutState::STARTED);
    spdlog::info("  Workput started");
}

auto Model::stopWorkout() -> void {
    std::lock_guard guard(mutex);

    spdlog::info("Stopping workout");
    storage->endWorkout();

    publishWorkoutEvent(WorkoutState::FINISHED);
    spdlog::info("  Workout stopped");
}

//
// Data retrieval
//

auto Model::tick() -> void {
    std::lock_guard guard(mutex);

    const auto now = system_clock::now();

    constexpr int MAX_RELEVANCE_SECONDS = 2;

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

    const auto timestamp = duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    storage->aggregate(timestamp, hrm, power, cadence, totalCrankRevs, speed, totalWheelRevs);

    publishWorkoutEvent(WorkoutState::IN_PROGRESS);
}

auto Model::getDeviceServices(const std::string &deviceId) -> std::vector<Service> {
    std::lock_guard guard(mutex);

    auto services = std::vector<Service>();

    if (hrmState.device && deviceId == hrmState.device->deviceId()) {
        services.push_back(Service::HEART_RATE);
    }

    if (cadenceState.device && deviceId == cadenceState.device->deviceId()) {
        services.push_back(Service::CADENCE);
    }

    if (speedState.device && deviceId == speedState.device->deviceId()) {
        services.push_back(Service::SPEED);
    }

    if (powerState.device && deviceId == powerState.device->deviceId()) {
        services.push_back(Service::POWER);
    }

    return services;
}

auto Model::getHrmDevice() const -> std::shared_ptr<Device> {
    return hrmState.device;
}

auto Model::getCadenceDevice() const -> std::shared_ptr<Device> {
    return cadenceState.device;
}

auto Model::getSpeedDevice() const -> std::shared_ptr<Device> {
    return speedState.device;
}

auto Model::getPowerDevice() const -> std::shared_ptr<Device> {
    return powerState.device;
}

auto Model::publishWorkoutEvent(const WorkoutState &status) -> void {
    std::lock_guard guard(mutex);

    const auto duration = status == WorkoutState::IN_PROGRESS
                              ? storage->getCurrentWorkoutDuration()
                              : storage->getTotalWorkoutDuration();

    const auto now = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch());
    auto data = status == WorkoutState::IN_PROGRESS
                    ? storage->getDataAt(now)
                    : storage->getWorkoutSummary();

    spdlog::trace("Speed: {}, avg Speed: {}, duration: {}", data.speed.value_or(0), data.speed_avg.value_or(0),
                  duration);

    auto distance_unit = distanceUnit;
    data.speed = data.speed.transform([distance_unit](const unsigned long x) {
        return x * getSpeedConversionFactor(distance_unit);
    });

    data.speed_avg = data.speed_avg.transform([distance_unit](const unsigned long x) {
        return x * getSpeedConversionFactor(distance_unit);
    });

    data.speed_min = data.speed_min.transform([distance_unit](const unsigned long x) {
        return x * getSpeedConversionFactor(distance_unit);
    });

    data.speed_max = data.speed_max.transform([distance_unit](const unsigned long x) {
        return x * getSpeedConversionFactor(distance_unit);
    });

    const auto distance = BLE::Math::computeDistance(data.speed_avg.value_or(0), duration);
    const auto summary = WorkoutEvent(status, duration, distance, distanceUnit, data);

    eventBus->publish(summary);
}
