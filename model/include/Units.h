#pragma once
#include <string>
#include <utility>

#include <fmt/core.h>

enum class DistanceUnit {
    METERS,
    MILES
};

inline auto distanceUnitToString(const DistanceUnit unit) -> const char * {
    switch (unit) {
        case DistanceUnit::METERS:
            return "Kilometers";
        case DistanceUnit::MILES:
            return "Miles";
        default:
            std::unreachable();
    }
}

inline auto distanceUnitToShortString(const DistanceUnit unit) -> const char * {
    switch (unit) {
        case DistanceUnit::METERS:
            return "km";
        case DistanceUnit::MILES:
            return "mi";
        default:
            std::unreachable();
    }
}

inline auto getSpeedConversionFactor(const DistanceUnit unit) -> double {
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
