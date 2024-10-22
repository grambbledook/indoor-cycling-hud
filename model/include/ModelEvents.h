#pragma once

#include <algorithm>
#include <iostream>

#include "Data.h"
#include "Service.h"
#include "Units.h"

class Event {
public:
    virtual ~Event() = default;
};

enum ConnectionStatus {
    CONNECTED,
    DISCONNECTED
};

struct DeviceConnectionEvent final : public Event {
    std::shared_ptr<Device> device;
    ConnectionStatus status;

    DeviceConnectionEvent(const std::shared_ptr<Device> &device, const ConnectionStatus status): device(device),
        status(status) {
    }
};

struct DeviceDiscovered final : public Event {
    std::shared_ptr<Device> device;

    explicit DeviceDiscovered(const std::shared_ptr<Device> &device): device(device) {
    }
};

struct DeviceSelected final : public Event {
    Service service;
    std::shared_ptr<Device> device;

    DeviceSelected(Service service, const std::shared_ptr<Device> &device): service(service), device(device) {
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
    ): state(state),
       durationMs(duration),
       distance(distance), distanceUnit(distanceUnit),
       data(data) {
    }
};
