#pragma once
#include <string>

enum class Service {
    HEART_RATE,
    CADENCE,
    SPEED,
    POWER,
    BIKE_TRAINER
};

inline std::string asString(const Service &as) {
    switch (as) {
        case Service::HEART_RATE: return "HEART_RATE";
        case Service::CADENCE: return "CADENCE";
        case Service::SPEED: return "SPEED";
        case Service::POWER: return "POWER";
        case Service::BIKE_TRAINER: return "BIKE_TRAINER";
        default: std::unreachable();
    }
}
