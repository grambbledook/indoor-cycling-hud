#include <gtest/gtest.h>

#include "DataFields.h"
#include "model/include/Units.h"

TEST(FormatDistanceTest, ShoudFormatDistance_Kilometers) {
    constexpr auto distance = 100000;
    constexpr auto unit = DistanceUnit::METERS;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "100.00 km");
}

TEST(FormatDistanceTest, ShoudFormatDistance_500_Kilometers) {
    constexpr auto distance = 500;
    constexpr auto unit = DistanceUnit::METERS;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "0.50 km");
}

TEST(FormatDistanceTest, ShoudFormatDistance_50_Kilometers) {
    constexpr auto distance = 50;
    constexpr auto unit = DistanceUnit::METERS;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "0.05 km");
}

TEST(FormatDistanceTest, ShoudFormatDistance_Miles) {
    constexpr auto distance = 100000;
    constexpr auto unit = DistanceUnit::MILES;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "100.00 mi");
}

TEST(FormatDistanceTest, ShoudFormatDistance_500_Miles) {
    constexpr auto distance = 500;
    constexpr auto unit = DistanceUnit::MILES;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "0.50 mi");
}

TEST(FormatDistanceTest, ShoudFormatDistance_50_Miles) {
    constexpr auto distance = 50;
    constexpr auto unit = DistanceUnit::MILES;

    const auto formatted = Data::formatDistance(distance, unit);
    ASSERT_EQ(formatted, "0.05 mi");
}
