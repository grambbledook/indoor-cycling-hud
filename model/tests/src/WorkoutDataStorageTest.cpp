#include <filesystem>
#include <tuple>

#include <gtest/gtest.h>
#include "WorkoutDataStorage.h"

using AggregateMethod = void (WorkoutDataStorage::*)(const long long, const long);
using RetrieveMethod = Aggregate (WorkoutDataStorage::*)();

class WorkoutDataStorageTestFixture
    : public ::testing::TestWithParam<std::tuple<std::string, AggregateMethod, RetrieveMethod>> {
protected:
    std::unique_ptr<WorkoutDataStorage> storage;

    void SetUp() override {
        storage = std::make_unique<WorkoutDataStorage>();
    }
};

TEST_P(WorkoutDataStorageTestFixture, ShouldSuccessfullyReadAggregatedData) {
    const auto &[type, aggregateMethod, retrieveMethod] = GetParam();

    (storage.get()->*aggregateMethod)(1, 10);
    (storage.get()->*aggregateMethod)(2, 100);

    const auto [val, avg] = (storage.get()->*retrieveMethod)();
    ASSERT_EQ(val, 100);
    ASSERT_EQ(avg, 55);
}

TEST_P(WorkoutDataStorageTestFixture, ShouldReturnEmptyAggregatedDataOnMissingData) {
    const auto &[type, aggregateMethod, retrieveMethod] = GetParam();

    const auto [val, avg] = (storage.get()->*retrieveMethod)();
    ASSERT_EQ(val, 0);
    ASSERT_EQ(avg, 0);
}

INSTANTIATE_TEST_SUITE_P(
    WorkoutDataStorageTests,
    WorkoutDataStorageTestFixture,
    ::testing::Values(
        std::make_tuple("hrm", &WorkoutDataStorage::aggregateHeartRate, &WorkoutDataStorage::getHeartRate),
        std::make_tuple("cad", &WorkoutDataStorage::aggregateCadence, &WorkoutDataStorage::getCadence),
        std::make_tuple("spd", &WorkoutDataStorage::aggregateSpeed, &WorkoutDataStorage::getSpeed),
        std::make_tuple("pwr", &WorkoutDataStorage::aggregatePower, &WorkoutDataStorage::getPower)
    )
);
