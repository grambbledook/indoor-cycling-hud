#pragma once
// #include "BluetoothServices.h"
#include "Service.h"

struct AppService {
    Service service;
};

static constexpr AppService HEART_RATE = {
    Service::HEART_RATE
};
static constexpr AppService CADENCE = {
    Service::CADENCE
};
static constexpr AppService SPEED = {
    Service::SPEED
};
static constexpr AppService POWER = {
    Service::POWER
};
static constexpr AppService BIKE_TRAINER = {
    Service::BIKE_TRAINER
};
