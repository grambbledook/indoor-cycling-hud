#include <gtest/gtest.h>
#include "Formula.h"
#include "WheelSizes.h"

TEST(ComputeCadenceTest, ShouldComputeCadence_FakeData) {
    constexpr auto startCCR = 0;
    constexpr auto endCCR = 1;

    constexpr auto startLCET = 0;
    constexpr auto endLCET = 1024;

    const auto cadence = BLE::Math::computeCadence(endLCET, startLCET, endCCR, startCCR);
    ASSERT_EQ(cadence, 60);
}

TEST(ComputeCadenceTest, ShouldComputeCadence_RealData) {
    constexpr auto startCCR = 131;
    constexpr auto endCCR = 132;

    constexpr auto startLCET = 52746;
    constexpr auto endLCET = 53600;

    const auto cadence = BLE::Math::computeCadence(endLCET, startLCET, endCCR, startCCR);
    ASSERT_EQ(cadence, 72);
}

TEST(ComputeCadenceTest, ShouldComputeCadence_CcrOverflow_FakeData) {
    constexpr auto startCCR = 0xFFFFFFFF;
    constexpr auto endCCR = 1;

    constexpr auto startLCET = 38642;
    constexpr auto endLCET = 40250;

    const auto cadence = BLE::Math::computeCadence(endLCET, startLCET, endCCR, startCCR);
    ASSERT_EQ(cadence, 76);
}

TEST(ComputeSpeedTest, ShouldComputeSpeed_FakeData) {
    constexpr auto startCWR = 0;
    constexpr auto endCWR = 3;

    constexpr auto startLWET = 0;
    constexpr auto endLWET = 1024;

    const auto wheel = getWheelCircumferenceInMM(WheelSize::ROAD_700x35C);
    const auto cadence = BLE::Math::computeSpeed(endLWET, startLWET, endCWR, startCWR, wheel);
    ASSERT_EQ(cadence, 651);
}

TEST(ComputeSpeedTest, ShouldComputeSpeed_RealData) {
    constexpr auto startCWR = 336920;
    constexpr auto endCWR = 336922;

    constexpr auto startLWET = 35624;
    constexpr auto endLWET = 36343;

    const auto wheel = getWheelCircumferenceInMM(WheelSize::ROAD_700x35C);
    const auto cadence = BLE::Math::computeSpeed(endLWET, startLWET, endCWR, startCWR, wheel);
    ASSERT_EQ(cadence, 618);
}

TEST(ComputeSpeedTest, ShouldComputeSpeed_LwetOverflow_RealData) {
    constexpr auto startCWR = 337845;
    constexpr auto endCWR = 337848;

    constexpr auto startLWET = 65300;
    constexpr auto endLWET = 646;

    const auto wheel = getWheelCircumferenceInMM(WheelSize::ROAD_700x35C);
    const auto cadence = BLE::Math::computeSpeed(endLWET, startLWET, endCWR, startCWR, wheel);
    ASSERT_EQ(cadence, 756);
}

TEST(ComputeDistanceTest, ShouldComputeDistance_Kilometers) {
    // 1 hour
    constexpr auto duration = 1000 * 60 * 60;
    // 100.0 km/h
    constexpr auto speed = 10000;

    const auto cadence = BLE::Math::computeDistance(speed, duration);
    ASSERT_EQ(cadence, 100000);
}

TEST(ComputeDistanceTest, ShouldComputeDistance_RealData_Kilometers) {
    // 1 hour
    constexpr auto duration = 18029;
    // 30.02 km/h
    constexpr auto speed = 3002;

    const auto cadence = BLE::Math::computeDistance(speed, duration);
    ASSERT_EQ(cadence, 150);
}

TEST(ComputeDistanceTest, ShouldComputeDistance_Miles) {
    // 1 hour
    constexpr auto duration = 1000 * 60 * 60;
    // 40.0 mph
    constexpr auto speed = 4000;

    const auto cadence = BLE::Math::computeDistance(speed, duration);
    ASSERT_EQ(cadence, 40000);
}
