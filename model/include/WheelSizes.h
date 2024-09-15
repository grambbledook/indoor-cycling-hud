#pragma once
#include <utility>
#include <fmt/core.h>
enum class WheelSize {
    ROAD_700x18C,
    ROAD_700x19C,
    ROAD_700x20C,
    ROAD_700x23C,
    ROAD_700x25C,
    ROAD_700x28C,
    ROAD_700x30C,
    ROAD_700x32C,
    ROAD_700x35C,
    ROAD_700x38C,
    ROAD_700x40C,
    ROAD_700x44C,
    ROAD_700x45C,
    ROAD_700x47C,
    ROAD_700x50C,
    ROAD_650x20C,
    ROAD_650x23C,
    ROAD_650x35A,
    ROAD_650x38B,
    ROAD_650x38A
};

inline int getWheelCircumferenceInMM(const WheelSize size) {
    switch (size) {
        case WheelSize::ROAD_700x18C:
            return 2070;
        case WheelSize::ROAD_700x19C:
            return 2080;
        case WheelSize::ROAD_700x20C:
            return 2086;
        case WheelSize::ROAD_700x23C:
            return 2096;
        case WheelSize::ROAD_700x25C:
            return 2109;
        case WheelSize::ROAD_700x28C:
            return 2127;
        case WheelSize::ROAD_700x30C:
            return 2140;
        case WheelSize::ROAD_700x32C:
            return 2152;
        case WheelSize::ROAD_700x35C:
            return 2171;
        case WheelSize::ROAD_700x38C:
            return 2190;
        case WheelSize::ROAD_700x40C:
            return 2203;
        case WheelSize::ROAD_700x44C:
            return 2230;
        case WheelSize::ROAD_700x45C:
            return 2234;
        case WheelSize::ROAD_700x47C:
            return 2247;
        case WheelSize::ROAD_700x50C:
            return 2265;
        case WheelSize::ROAD_650x20C:
            return 1938;
        case WheelSize::ROAD_650x23C:
            return 1944;
        case WheelSize::ROAD_650x35A:
            return 2090;
        case WheelSize::ROAD_650x38B:
            return 2105;
        case WheelSize::ROAD_650x38A:
            return 2125;
        default:
            std::unreachable();
    }
}

inline const char *toWheelSizeString(const WheelSize size) {
    switch (size) {
        case WheelSize::ROAD_700x18C:
            return "700 x 18C (18-622)";
        case WheelSize::ROAD_700x19C:
            return "700 x 19C (19-622)";
        case WheelSize::ROAD_700x20C:
            return "700 x 20C (20-622)";
        case WheelSize::ROAD_700x23C:
            return "700 x 23C (23-622)";
        case WheelSize::ROAD_700x25C:
            return "700 x 25C (25-622)";
        case WheelSize::ROAD_700x28C:
            return "700 x 28C (28-622)";
        case WheelSize::ROAD_700x30C:
            return "700 x 30C (30-622)";
        case WheelSize::ROAD_700x32C:
            return "700 x 32C (32-622)";
        case WheelSize::ROAD_700x35C:
            return "700 x 35C (35-622)";
        case WheelSize::ROAD_700x38C:
            return "700 x 38C (38-622)";
        case WheelSize::ROAD_700x40C:
            return "700 x 40C (40-622)";
        case WheelSize::ROAD_700x44C:
            return "700 x 44C (44-622)";
        case WheelSize::ROAD_700x45C:
            return "700 x 45C (45-622)";
        case WheelSize::ROAD_700x47C:
            return "700 x 47C (47-622)";
        case WheelSize::ROAD_700x50C:
            return "700 x 50C (50-622)";
        case WheelSize::ROAD_650x20C:
            return "650 x 20C (20-571)";
        case WheelSize::ROAD_650x23C:
            return "650 x 23C (23-571)";
        case WheelSize::ROAD_650x35A:
            return "650 x 35A (37-590)";
        case WheelSize::ROAD_650x38B:
            return "650 x 38B (40-584)";
        case WheelSize::ROAD_650x38A:
            return "650 x 38A (40-590)";
        default:
            std::unreachable();
    }
};

template<>
struct fmt::formatter<WheelSize> {
    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const WheelSize &wheel_size, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "{}", toWheelSizeString(wheel_size));
    }
};
