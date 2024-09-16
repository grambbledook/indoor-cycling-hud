#pragma once
#include <string>
#include <utility>

#include <fmt/core.h>

enum class DistanceUnit {
    METERS,
    MILES
};

inline const char *distanceUnitToString(const DistanceUnit unit) {
    switch (unit) {
        case DistanceUnit::METERS:
            return "Kilometers";
        case DistanceUnit::MILES:
            return "Miles";
        default:
            std::unreachable();
    }
}

inline const char *distanceUnitToShortString(const DistanceUnit unit) {
    switch (unit) {
        case DistanceUnit::METERS:
            return "km";
        case DistanceUnit::MILES:
            return "mi";
        default:
            std::unreachable();
    }
}

inline double getSpeedConversionFactor(const DistanceUnit unit) {
    switch (unit) {
        // 1 m/s = 3.6 km/h
        case DistanceUnit::METERS:
            return 3.6;
        // 1 m/s = 2.23694 mph
        case DistanceUnit::MILES:
            return 2.23694;
        default:
            std::unreachable();
    }
};

inline double getDistanceConversionFactor(const DistanceUnit unit) {
    switch (unit) {

        case DistanceUnit::METERS:
            return 1e-7;
        case DistanceUnit::MILES:
            return 1.0 / 16093400.0;
        default:
            std::unreachable();
    }
};

template<>
struct fmt::formatter<DistanceUnit> {
    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    };

    template<typename FormatContext>
    auto format(const DistanceUnit &speed_unit, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "{}", distanceUnitToString(speed_unit));
    };
};