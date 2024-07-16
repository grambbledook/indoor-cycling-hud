#pragma once
#include <string>
#include <unordered_set>

struct Service {
    const std::string type;
    const std::string service_uuid;
    const std::string characteristic_uuid;
};

const auto HRM = Service(
    "Heart Rate Monitor",
    "0000180d-0000-1000-8000-00805f9b34fb",
    "00002a37-0000-1000-8000-00805f9b34fb"
);

const auto CSC = Service(
    "Cadence & Speed Sensor",
    "00001816-0000-1000-8000-00805f9b34fb",
    "00002a5b-0000-1000-8000-00805f9b34fb"
);

const auto PWR = Service(
    "Power Meter",
    "00001818-0000-1000-8000-00805f9b34fb",
    "00002a63-0000-1000-8000-00805f9b34fb"
);

const auto LEGACY_BIKE_TRAINER = Service(
    "Bike Trainer (FE-C over Bluetooth)",
    "6e40fec1-b5a3-f393-e0a9-e50e24dcca9e",
    "6e40fec2-b5a3-f393-e0a9-e50e24dcca9e"
);

const std::unordered_set SUPPORTED_SERVICES = {HRM, CSC, PWR, LEGACY_BIKE_TRAINER};
