#pragma once

#include <algorithm>
#include <iostream>

#include "Data.h"
#include "Service.h"

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
    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};

struct WorkoutEvent {
    bool isFinished;

    long long duration;
    Aggregate hrm;
    Aggregate cadence;
    Aggregate speed;
    Aggregate power;
};
