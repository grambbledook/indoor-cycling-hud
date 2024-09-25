#pragma once

#include <algorithm>
#include <iostream>

#include "Data.h"
#include "Service.h"
#include "Units.h"

struct DeviceDiscovered {
    std::shared_ptr<Device> device;
};

struct DeviceSelected {
    Service service;
    std::shared_ptr<Device> device;

    DeviceSelected(Service service, std::shared_ptr<Device> device): service(service), device(device) {
    }
};

struct Aggregate2 {
     std::optional<unsigned long> hrm;
     std::optional<unsigned long> hrm_avg;
     std::optional<unsigned long> hrm_min;
     std::optional<unsigned long> hrm_max;
     std::optional<unsigned long> power;
     std::optional<unsigned long> power_avg;
     std::optional<unsigned long> power_3s;
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

struct Aggregate {
    unsigned long val;
    unsigned long avg;
    unsigned long windowedAvg;
    unsigned long max;
    unsigned long min;
};

enum class WorkoutState {
    STARTED,
    IN_PROGRESS,
    FINISHED
};

struct WorkoutEvent {
    WorkoutState state;

    long long durationMs;
    unsigned long distance;
    DistanceUnit distanceUnit;

    Aggregate2 data;
    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};
