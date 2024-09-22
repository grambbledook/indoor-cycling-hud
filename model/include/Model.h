#pragma once
#include <expected>
#include <mutex>

#include "Channel.h"
#include "ModelEvents.h"
#include "BleDeviceEvents.h"
#include "Units.h"
#include "WheelSizes.h"
#include "WorkoutDataStorage.h"

using std::chrono::system_clock;

class Notifications {
public:
    Channel<DeviceDiscovered> deviceDiscovered;
    Channel<DeviceSelected> deviceSelected;
    Channel<WorkoutEvent> measurements;
    Channel<WorkoutEvent> summary;
};

template<typename A>
struct State {
    std::shared_ptr<Device> device;
    std::vector<A> data;

    std::expected<std::vector<A>, void *> getLastN(int n) {
        if (n > data.size()) {
            return std::unexpected<void *>(nullptr);
        }

        return {std::vector<A>(data.end() - n, data.end())};
    }

    auto recordMetric(A value) -> void {
        if (data.size() == 2) {
            data.erase(data.begin());
        }
        data.push_back(value);
    }

    auto unrecordMetric() -> void {
        if (data.empty()) {
            return;
        }
        data.erase(data.end() - 1);
    };
};

struct MeasurementsBuffer {
    struct Record {
        unsigned long value;
        system_clock::time_point timestamp;
    };

    Record heartRate;
    Record cadence;
    Record speed;
    Record power;
};

class Model {
public:
    Model(): distanceUnit(DistanceUnit::METERS),
             wheelSize(WheelSize::ROAD_700x35C),
             storage(std::make_unique<WorkoutDataStorage>()) {
    }

    auto addDevice(const std::shared_ptr<Device> &device) -> void;

    auto getDevices(const GattService *service) -> std::vector<std::shared_ptr<Device> >;

    auto setDevice(const std::shared_ptr<Device> &device) -> void;

    auto setHeartRateMonitor(const std::shared_ptr<Device> &device) -> void;

    auto setCadenceSensor(const std::shared_ptr<Device> &device) -> void;

    auto setSpeedSensor(const std::shared_ptr<Device> &device) -> void;

    auto setPowerMeter(const std::shared_ptr<Device> &device) -> void;

    auto setBikeTrainer(const std::shared_ptr<Device> &device) -> void;

    auto setSpeedUnit(DistanceUnit unit) -> void;

    auto setWheelSize(WheelSize size) -> void;

    auto startWorkout() -> void;

    auto stopWorkout() -> void;

    auto recordHeartData(const MeasurementEvent<HrmMeasurement> &event) -> void;

    auto recordCadenceData(const MeasurementEvent<CadenceMeasurement> &event) -> void;

    auto recordSpeedData(const MeasurementEvent<SpeedMeasurement> &event) -> void;

    auto recordPowerData(const MeasurementEvent<PowerMeasurement> &event) -> void;

    auto recordTrainerData(const MeasurementEvent<GeneralData> &event) -> void;

    auto recordTrainerData(const MeasurementEvent<GeneralSettings> &event) -> void;

    auto recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event) -> void;

    auto tick() -> void;

private:
    auto publishWorkoutEvent(WorkoutState status, Channel<WorkoutEvent> &channel) -> void;

public:
    Notifications notifications;

private:
    std::unordered_map<std::string, std::shared_ptr<Device> > devices;
    std::recursive_mutex mutex;

    DistanceUnit distanceUnit;
    WheelSize wheelSize;
    std::unique_ptr<WorkoutDataStorage> storage;

    MeasurementsBuffer buffer = {
        0, system_clock::now(),
        0, system_clock::now(),
        0, system_clock::now(),
        0, system_clock::now(),
    };

    State<int> hrmState = {
        __nullptr,
        std::vector<int>{}
    };

    State<std::pair<unsigned long, unsigned long> > cadenceState = {
        __nullptr,
        std::vector<std::pair<unsigned long, unsigned long> >{}
    };

    State<std::pair<unsigned long, unsigned long> > speedState = {
        __nullptr,
        std::vector<std::pair<unsigned long, unsigned long> >{}
    };

    State<int> powerState = {
        __nullptr,
        std::vector<int>{}
    };
};
