#pragma once

#include <algorithm>

#include "Data.h"
#include "Service.h"
#include "Units.h"

enum EventType {
    DEVICE_CONNECTION,
    DEVICE_DISCOVERED,
    DEVICE_SELECTED,
    WORKOUT_DATA

};

class Event {
public:
    explicit Event(const EventType type): type(type) {
    }

    virtual ~Event() = default;

    EventType type;
};

enum ConnectionStatus {
    CONNECTED,
    DISCONNECTED
};

struct DeviceConnectionEvent final : Event {
    std::shared_ptr<Device> device;
    ConnectionStatus status;

    DeviceConnectionEvent(
        const std::shared_ptr<Device> &device,
        const ConnectionStatus status
    ): Event(DEVICE_CONNECTION), device(device), status(status) {
    }
};

struct DeviceDiscovered final : Event {
    std::shared_ptr<Device> device;

    explicit DeviceDiscovered(const std::shared_ptr<Device> &device): Event(DEVICE_DISCOVERED), device(device) {
    }
};

struct DeviceSelected final : Event {
    Service service;
    std::shared_ptr<Device> device;

    DeviceSelected(
        const Service &service,
        const std::shared_ptr<Device> &device
    ): Event(DEVICE_SELECTED), service(service), device(device) {
    }
};

enum class WorkoutState {
    STARTED,
    IN_PROGRESS,
    FINISHED
};

struct Aggregate {
    std::optional<unsigned long> hrm;
    std::optional<unsigned long> hrm_avg;
    std::optional<unsigned long> hrm_min;
    std::optional<unsigned long> hrm_max;
    std::optional<unsigned long> power;
    std::optional<unsigned long> power_3s;
    std::optional<unsigned long> power_avg;
    std::optional<unsigned long> power_min;
    std::optional<unsigned long> power_max;
    std::optional<unsigned long> cadence;
    std::optional<unsigned long> cadence_avg;
    std::optional<unsigned long> cadence_min;
    std::optional<unsigned long> cadence_max;
    std::optional<unsigned long> speed;
    std::optional<unsigned long> speed_avg;
    std::optional<unsigned long> speed_min;
    std::optional<unsigned long> speed_max;
};

struct WorkoutEvent final : public Event {
    WorkoutState state;

    long long durationMs;
    unsigned long distance;
    DistanceUnit distanceUnit;

    Aggregate data;

    WorkoutEvent(
        const WorkoutState state,
        const long long duration,
        const unsigned long distance,
        const DistanceUnit distanceUnit,
        const Aggregate &data
    ): Event(WORKOUT_DATA),
       state(state),
       durationMs(duration),
       distance(distance), distanceUnit(distanceUnit),
       data(data) {
    }
};
