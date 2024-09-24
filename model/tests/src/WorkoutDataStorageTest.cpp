#include <filesystem>
#include <tuple>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include "WorkoutDataStorage.h"

TEST(WorkoutDataStorageTest, ShouldSuccesfullyAggregateData) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();
    storage->aggregate(timestamp, 10, 10, 10, 10, 10, 10);

    const auto aggregate = storage->getDataAt(timestamp);

    ASSERT_EQ(aggregate.hrm, 10);
    ASSERT_EQ(aggregate.hrm_avg, 10);
    ASSERT_EQ(aggregate.cadence, 10);
    ASSERT_EQ(aggregate.cadence_avg, 10);
    ASSERT_EQ(aggregate.speed, 10);
    ASSERT_EQ(aggregate.speed_avg, 10);
    ASSERT_EQ(aggregate.power, 10);
    ASSERT_EQ(aggregate.power_avg, 10);
    ASSERT_EQ(aggregate.power_3s, 10);
}

TEST(WorkoutDataStorageTest, ShouldSuccesfullyComputeAverage) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();

    storage->aggregate(timestamp - std::chrono::seconds(4), 10, 10, 10, 10, 10, 10);
    storage->aggregate(timestamp - std::chrono::seconds(1), 20, 20, 20, 20, 20, 20);
    storage->aggregate(timestamp, 60, 60, 60, 60, 60, 60);

    const auto aggregate = storage->getDataAt(timestamp);

    ASSERT_EQ(aggregate.hrm, 60);
    ASSERT_EQ(aggregate.hrm_avg, 30);
    ASSERT_EQ(aggregate.cadence, 60);
    ASSERT_EQ(aggregate.cadence_avg, 30);
    ASSERT_EQ(aggregate.speed, 60);
    ASSERT_EQ(aggregate.speed_avg, 30);
    ASSERT_EQ(aggregate.power, 60);
    ASSERT_EQ(aggregate.power_avg, 30);
    ASSERT_EQ(aggregate.power_3s, 40);
}

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

TEST(WorkoutDataStorageTest, ShouldAggregateDataAfterEachRequest) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateHeartRate(1);
    auto result = storage->getHeartRate();
    ASSERT_EQ(result.avg, 1);

    storage->aggregateHeartRate(2);
    result = storage->getHeartRate();
    ASSERT_EQ(result.avg, 2);

    storage->aggregateHeartRate(2);
    result = storage->getHeartRate();
    ASSERT_EQ(result.avg, 2);
}

TEST(WorkoutDataStorageTest, ShouldReadAggregatedData_DURATION) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateHeartRate(10);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    storage->aggregateHeartRate(100);

    const auto duration = storage->getTotalWorkoutDuration();
    ASSERT_GE(duration, 100);
    ASSERT_LE(duration, 1000);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_DURATION) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto duration = storage->getTotalWorkoutDuration();
    ASSERT_EQ(duration, 0);
}

