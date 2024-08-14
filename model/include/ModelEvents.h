#pragma once

#include <algorithm>
#include <iostream>

#include "Data.h"
#include "Service.h"

struct Statistics {
    int latest;
    int min;
    int max;
    int average;
    int count;

    void aggregate(const int value) {
        latest = value;
        min = std::min(min, value);
        max = std::max(max, value);
        average = (average * count + value) / (count + 1);
        count++;
    }
};

struct DeviceDiscovered {
    std::shared_ptr<Device> device;
};

struct DeviceSelected {
    Service service;
    std::shared_ptr<Device> device;

    // DeviceSelected(const DeviceSelected &event) = default;
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
