#include <filesystem>
#include <tuple>

#include <gtest/gtest.h>
#include "WorkoutDataStorage.h"

TEST(WorkoutDataStorageTest, ShouldSuccessfullyReadAggregatedData_HRM) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateHeartRate(10);
    storage->aggregateHeartRate(100);

    const auto [val, avg, windowAvg, max, min] = storage->getHeartRate();
    ASSERT_EQ(val, 100);
    ASSERT_EQ(avg, 55);
    ASSERT_EQ(windowAvg, 55);
    ASSERT_EQ(max, 100);
    ASSERT_EQ(min, 10);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_HRM) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto [val, avg, windowAvg, max, min] = storage->getHeartRate();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
    ASSERT_EQ(windowAvg, 0);
    ASSERT_EQ(max, 0);
    ASSERT_EQ(min, 0);
}

TEST(WorkoutDataStorageTest, ShouldSuccessfullyReadAggregatedData_CADENCE) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateCadence(10);
    storage->aggregateCadence(100);

    const auto [val, avg, windowAvg, max, min] = storage->getHeartRate();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
    ASSERT_EQ(windowAvg, 0);
    ASSERT_EQ(max, 0);
    ASSERT_EQ(min, 0);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_CADENCE) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto [val, avg, windowAvg, max, min] = storage->getHeartRate();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
    ASSERT_EQ(windowAvg, 0);
    ASSERT_EQ(max, 0);
    ASSERT_EQ(min, 0);
}

TEST(WorkoutDataStorageTest, ShouldSuccessfullyReadAggregatedData_SPEED) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateSpeed(10);
    storage->aggregateSpeed(100);

    const auto [val, avg, windowAvg, max, min] = storage->getSpeed();
    ASSERT_EQ(val, 100);
    ASSERT_EQ(avg, 55);
    ASSERT_EQ(windowAvg, 55);
    ASSERT_EQ(max, 100);
    ASSERT_EQ(min, 10);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_SPEED) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto [val, avg, windowAvg, max, min] = storage->getSpeed();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
    ASSERT_EQ(windowAvg, 0);
    ASSERT_EQ(max, 0);
    ASSERT_EQ(min, 0);
}

TEST(WorkoutDataStorageTest, ShouldSuccessfullyReadAggregatedData_POWER) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregatePower(10);
    storage->aggregatePower(100);

    const auto [val, avg, windowAvg, max, min] = storage->getPower();
    ASSERT_EQ(val, 100);
    ASSERT_EQ(avg, 55);
    ASSERT_EQ(windowAvg, 55);
    ASSERT_EQ(max, 100);
    ASSERT_EQ(min, 10);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_POWER) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto [val, avg, windowAvg, max, min] = storage->getHeartRate();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
    ASSERT_EQ(windowAvg, 0);
    ASSERT_EQ(max, 0);
    ASSERT_EQ(min, 0);
}

