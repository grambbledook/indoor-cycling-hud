#pragma once
#include <sqlite3.h>
#include <sqlite_utils.h>
#include <filesystem>

#include <stdexcept>
#include <string>

#include "ModelEvents.h"

constexpr auto create_table_sql = R"CREATE(
    CREATE TABLE measurements (
        ts INTEGER PRIMARY KEY,
        value INTEGER,
        type VARCHAR(3)
    );

    CREATE INDEX idx_ts_type ON measurements(ts, type) WHERE type IS NOT NULL;
)CREATE";

constexpr auto insert_sql = R"INSERT(
    INSERT INTO measurements (ts, value, type)
    VALUES (?, ?, ?);
)INSERT";

constexpr auto select_sql = R"QUERY(
    SELECT
        value,
        AVG(value) AS avg_value,
        MIN(value) AS min_value,
        MAX(value) AS max_value
    FROM measurements
    WHERE type = ?
    GROUP BY type;
)QUERY";


class WorkoutDataStorage {
public:
    WorkoutDataStorage();

    ~WorkoutDataStorage();

    void aggregateHeartRate(long long ts, long val);

    Aggregate getHeartRate();

    void aggregateCadence(long long timestamp, long measurement);

    Aggregate getCadence();

    void aggregateSpeed(long long timestamp, long measurement);

    Aggregate getSpeed();

    void aggregatePower(long long timestamp, long measurement);

    Aggregate getPower();

private:
    void insert(long long ts, long value, const std::string &type);

    Aggregate select(const std::string &type);

private:
    static long id;
    std::unique_ptr<SQLiteConnection> connection;
    std::filesystem::path db_file;
};
