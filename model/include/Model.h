#pragma once
#include <mutex>

#include "Channel.h"
#include "ModelEvents.h"
#include "BleDeviceEvents.h"
#include "Units.h"
#include "WheelSizes.h"
#include "WorkoutDataStorage.h"

class Notifications {
public:
    Channel<DeviceDiscovered> deviceDiscovered;
    Channel<DeviceSelected> deviceSelected;
    Channel<WorkoutData> measurements;
    Channel<WorkoutEvent> summary;
};

template<typename A>
struct State {
    std::shared_ptr<Device> device;
    std::vector<A> data;

    std::pair<std::vector<A>, bool> getLastN(int n) {
        if (n > data.size()) {
            return {std::vector<A>(n), false};
        }

        return {std::vector<A>(data.end() - n, data.end()), true};
    }

    void recordMetric(A value) {
        if (data.size() == 2) {
            data.erase(data.begin());
        }
        data.push_back(value);
    }

    void unrecordMetric() {
        if (data.empty()) {
            return;
        }
        data.erase(data.end() - 1);
    };
};


class Model {
public:
    Model(): distanceUnit(DistanceUnit::METERS),
             wheelSize(WheelSize::ROAD_700x35C),
             storage(std::make_unique<WorkoutDataStorage>()) {
    }

    void addDevice(const std::shared_ptr<Device> &device);

    std::vector<std::shared_ptr<Device> > getDevices(const GattService *service);

    void setDevice(const std::shared_ptr<Device> &device);

    void setHeartRateMonitor(const std::shared_ptr<Device> &device);

    void setCadenceSensor(const std::shared_ptr<Device> &device);

    void setSpeedSensor(const std::shared_ptr<Device> &device);

    void setPowerMeter(const std::shared_ptr<Device> &device);

    void setBikeTrainer(const std::shared_ptr<Device> &device);

    void setSpeedUnit(DistanceUnit unit);

    void setWheelSize(WheelSize size);

    void startWorkout();

    void stopWorkout();

    void recordHeartData(const MeasurementEvent<HrmMeasurement> &event);

    void recordCadenceData(const MeasurementEvent<CadenceMeasurement> &event);

    void recordSpeedData(const MeasurementEvent<SpeedMeasurement> &event);

    void recordPowerData(const MeasurementEvent<PowerMeasurement> &event);

    void recordTrainerData(const MeasurementEvent<GeneralData> &event);

    void recordTrainerData(const MeasurementEvent<GeneralSettings> &event);

    void recordTrainerData(const MeasurementEvent<SpecificTrainerData> &event);

private:
    void publishUpdate();

public:
    Notifications notifications;

private:
    std::unordered_map<std::string, std::shared_ptr<Device> > devices;
    std::recursive_mutex mutex;

    DistanceUnit distanceUnit;
    WheelSize wheelSize;
    std::unique_ptr<WorkoutDataStorage> storage;

    State<int> hrmState = {
        __nullptr,
        std::vector<int>{}
    };

    State<std::pair<uint32_t, uint32_t> > cadenceState = {
        __nullptr,
        std::vector<std::pair<uint32_t, uint32_t> >{}
    };

    State<std::pair<uint32_t, uint32_t> > speedState = {
        __nullptr,
        std::vector<std::pair<uint32_t, uint32_t> >{}
    };

    State<int> powerState = {
        __nullptr,
        std::vector<int>{}
    };
};
