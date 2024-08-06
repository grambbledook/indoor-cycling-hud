#pragma once
#include <sqlite3.h>
#include <filesystem>

#include "ModelEvents.h"

class WorkoutDataStorage {
public:
    WorkoutDataStorage();

    ~WorkoutDataStorage();

    void aggregateHeartRate(long long timestamp, long measurement);

    Aggregate getHeartRate();

    void aggregateCadence(long long timestamp, long measurement);

    Aggregate getCadence();

    void aggregateSpeed(long long timestamp, long measurement);

    Aggregate getSpeed();

    void aggregatePower(long long timestamp, long measurement);

    Aggregate getPower();

    struct sqlite3_deleter {
        void operator()(sqlite3 *db) const {
            sqlite3_free(db);
        }
    };

private:
    static long id;
    std::unique_ptr<sqlite3, sqlite3_deleter> connection;
    std::filesystem::path db_file;
};
