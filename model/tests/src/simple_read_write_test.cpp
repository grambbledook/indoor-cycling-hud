#include <gtest/gtest.h>
#include <sqlite3.h>
#include <filesystem>


#include "WorkoutDataStorage.h"

TEST(ModelTest, SimpleReadWriteTest) {
    const auto storage = std::make_unique<WorkoutDataStorage>();

    storage->aggregateHeartRate(1, 100);
    storage->aggregateHeartRate(2, 110);

    const auto [val, avg] = storage->getHeartRate();
    ASSERT_EQ(val, 110);
    ASSERT_EQ(avg, 105);
}