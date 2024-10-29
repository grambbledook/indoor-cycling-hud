#pragma once
#include <expected>
#include <mutex>

#include "ModelEvents.h"
#include "BleDeviceEvents.h"
#include "EventBus.h"
#include "Units.h"
#include "WheelSizes.h"
#include "WorkoutDataStorage.h"

using std::chrono::system_clock;

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
    template<typename T>
    struct Record {
        T value;
        system_clock::time_point timestamp;
    };

    Record<unsigned long> heartRate;
    Record<std::tuple<long, unsigned long> > cadence;
    Record<std::tuple<long, unsigned long> > speed;
    Record<unsigned long> power;
};

class Model {
public:
    explicit Model(const std::shared_ptr<EventBus> &eventBus): distanceUnit(DistanceUnit::METERS),
                                                               eventBus(eventBus),
                                                               wheelSize(WheelSize::ROAD_700x35C),
                                                               storage(std::make_unique<WorkoutDataStorage>()) {
    }


    // Device discovery

    auto addDevice(const std::shared_ptr<Device> &device) -> void;

    auto getDevices(const GattService *service) -> std::vector<std::shared_ptr<Device> >;

    auto getDeviceServices(const std::shared_ptr<Device> &shared) -> std::vector<Service>;


    // Device modificators
    auto setDevice(const Service &service, const std::shared_ptr<Device> &device) -> void;

    auto setSpeedUnit(DistanceUnit unit) -> void;

    auto setWheelSize(WheelSize size) -> void;


    // Data recording
    auto recordData(const MeasurementEvent &event) -> void;


    // Workout management
    auto startWorkout() -> void;

    auto stopWorkout() -> void;


    // Data retrieval
    auto tick() -> void;

private:

    // Device setters
    auto recordHeartData(const HrmMeasurement &hrm) -> void;

    auto recordCadenceData(const CadenceMeasurement &event) -> void;

    auto recordSpeedData(const SpeedMeasurement &event) -> void;

    auto recordPowerData(const PowerMeasurement &event) -> void;


    // Data propagation
    auto publishWorkoutEvent(const WorkoutState &status) -> void;

private:
    std::shared_ptr<EventBus> eventBus;
    std::unordered_map<std::string, std::shared_ptr<Device> > devices;
    std::recursive_mutex mutex;

    DistanceUnit distanceUnit;
    WheelSize wheelSize;
    std::unique_ptr<WorkoutDataStorage> storage;

    MeasurementsBuffer buffer = {
        {0, system_clock::now()},
        {{}, system_clock::now()},
        {{}, system_clock::now()},
        {0, system_clock::now()},
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
