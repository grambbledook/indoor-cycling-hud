#include <filesystem>
#include <tuple>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include "WorkoutDataStorage.h"

inline std::chrono::milliseconds toMillis(std::chrono::duration<long long, std::ratio<1, 10000000> > duration) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

TEST(WorkoutDataStorageTest, ShouldSuccesfullyAggregateData) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();
    storage->aggregate(toMillis(timestamp), 10, 10, 10, 10, 10, 10);

    const auto aggregate = storage->getDataAt(toMillis(timestamp));

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

TEST(WorkoutDataStorageTest, ShouldSuccesfullyComputeAverages) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();

    const auto first = toMillis(timestamp - std::chrono::seconds(4));
    storage->aggregate(first, 10, 10, 10, 10, 10, 10);

    const auto second = toMillis(timestamp - std::chrono::seconds(1));
    storage->aggregate(second, 20, 20, 20, 20, 20, 20);

    const auto third = toMillis(timestamp);
    storage->aggregate(third, 60, 60, 60, 60, 60, 60);

    const auto aggregate = storage->getDataAt(toMillis(timestamp));

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

TEST(WorkoutDataStorageTest, ShouldSuccesfullyReadMDataIfSomeFieldsMissing) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();

    storage->aggregate(toMillis(timestamp), 10, 0, 0, 0, 0, 0);

    const auto aggregate = storage->getDataAt(toMillis(timestamp));

    ASSERT_EQ(aggregate.hrm, 10);
    ASSERT_EQ(aggregate.hrm_avg, 10);
    ASSERT_FALSE(aggregate.cadence.has_value());
    ASSERT_FALSE(aggregate.cadence_avg);
    ASSERT_FALSE(aggregate.speed);
    ASSERT_FALSE(aggregate.speed_avg);
    ASSERT_FALSE(aggregate.power);
    ASSERT_FALSE(aggregate.power_avg);
    ASSERT_FALSE(aggregate.power_3s);
}


TEST(WorkoutDataStorageTest, ShouldSuccesfullyAggegateDataIfSomeFieldsMissing) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();

    const auto first = toMillis(timestamp - std::chrono::seconds(4));
    storage->aggregate(first, 10, 0, 0, 10, 0, 10);

    const auto second = toMillis(timestamp - std::chrono::seconds(3));
    storage->aggregate(second, 0, 20, 0, 20, 0, 20);

    const auto third = toMillis(timestamp - std::chrono::seconds(2));
    storage->aggregate(third, 10, 20, 0, 30, 0, 30);

    const auto fourth = toMillis(timestamp - std::chrono::seconds(1));
    storage->aggregate(fourth, 10, 20, 0, 40, 0, 40);

    const auto aggregate = storage->getDataAt(toMillis(timestamp));

    ASSERT_EQ(aggregate.hrm, 10);
    ASSERT_EQ(aggregate.hrm_avg, 10);
    ASSERT_FALSE(aggregate.cadence.has_value());
    ASSERT_FALSE(aggregate.cadence_avg.has_value());
    ASSERT_EQ(aggregate.speed, 40);
    ASSERT_EQ(aggregate.speed_avg, 25);
    ASSERT_EQ(aggregate.power, 20);
    ASSERT_EQ(aggregate.power_avg, 20);
    ASSERT_EQ(aggregate.power_3s, 20);
}

TEST(WorkoutDataStorageTest, ShouldRead_DURATION) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto now = std::chrono::system_clock::now();

    const auto first = now - std::chrono::seconds(10);
    storage->aggregate(toMillis(first.time_since_epoch()), 0, 0, 0, 0, 0, 0);

    const auto second = now + std::chrono::seconds(10);
    storage->aggregate(toMillis(second.time_since_epoch()), 0, 0, 0, 0, 0, 0);

    const auto duration = storage->getTotalWorkoutDuration();
    ASSERT_EQ(duration, 20 * 1000);
}

TEST(WorkoutDataStorageTest, ShouldReturnEmptyAggregatedDataOnMissingData_DURATION) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    const auto duration = storage->getTotalWorkoutDuration();
    ASSERT_EQ(duration, 0);
}
