#pragma once// Add the Unit enum to `Units.h`
#include <string>
#include <utility>

#include "Formula.h"

enum class SpeedUnit {
    KMH,
    MPH
};

inline const char *speedUnitToString(const SpeedUnit unit) {
    switch (unit) {
        case SpeedUnit::KMH:
            return "km/h";
        case SpeedUnit::MPH:
            return "mph";
        default:
            std::unreachable();
    }
}

inline double getSpeedConversionFactor(const SpeedUnit unit) {
    switch (unit) {
        // 1 m/s = 3.6 km/h
        case SpeedUnit::KMH:
            return 3.6;
        // 1 m/s = 2.23694 mph
        case SpeedUnit::MPH:
            return 2.23694;
        default:
            std::unreachable();
    }
}
