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

    DeviceSelected(Service service, std::shared_ptr<Device> device): service(service), device(device) {}
};

struct Aggregate {
    int val;
    int avg;
    int windowedAvg;
    int max;
    int min;
};

struct WorkoutData {
    long long duration;
    long distance;
    DistanceUnit distanceUnit;

    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};

struct WorkoutEvent {
    bool isFinished;

    long long durationMs;
    long distance;
    DistanceUnit distanceUnit;

    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};
