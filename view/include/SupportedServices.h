#pragma once
// #include "BluetoothServices.h"
#include "Service.h"

struct AppService {
    Service service;
    // GattService gattService;
};

static const AppService HEART_RATE = {
    Service::HEART_RATE//,
    // BLE::Services::HRM
};
static const AppService CADENCE = {
    Service::CADENCE//,
    // BLE::Services::CSC
};
static const AppService SPEED = {
    Service::SPEED//,
    // BLE::Services::CSC
};
static const AppService POWER = {
    Service::POWER//,
    // BLE::Services::CSC
};
static const AppService BIKE_TRAINER = {
    Service::BIKE_TRAINER//,
    // BLE::Services::FEC_BIKE_TRAINER
};
