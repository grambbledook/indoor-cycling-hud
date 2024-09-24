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
    unsigned long hrm;
    unsigned long hrm_avg;
    unsigned long power;
    unsigned long power_avg;
    unsigned long power_3s;
    unsigned long cadence;
    unsigned long cadence_avg;
    unsigned long speed;
    unsigned long speed_avg;
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

    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};
