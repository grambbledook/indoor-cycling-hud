#pragma once
#include <sqlite3.h>
#include <sqlite_utils.h>
#include <filesystem>

#include <stdexcept>
#include <string>

#include "ModelEvents.h"

constexpr auto create_tables_sql = R"CREATE(
    CREATE TABLE measurements (
        ts TIMESTAMP PRIMARY KEY,
        value INTEGER,
        count INTEGER,
        avg REAL,
        max INTEGER,
        min INTEGER,
        type VARCHAR(3)
    );

    CREATE INDEX idx_ts_type ON measurements(ts, type) WHERE type IS NOT NULL;
)CREATE";

constexpr auto insert_sql = R"INSERT(
    INSERT INTO measurements (ts, type, value, count, avg, max, min)
    VALUES (datetime('now', 'subsecond'), ?, ?, ?, ?, ?, ?);
)INSERT";

constexpr auto select_latest_sql = R"QUERY(
    SELECT
        value AS value,
        count AS count,
        avg AS avg,
        max AS max,
        min AS min
    FROM
        measurements
    WHERE type = ?
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

constexpr auto select_aggregate_sql = R"QUERY(
    SELECT
        value AS value,
        avg AS avg,
        AVG(value) OVER () AS three_sec_avg,
        max AS max,
        min AS min
    FROM
        measurements
    WHERE type = ?
        AND ts >= datetime('now', '-3 seconds', 'subsecond')
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

constexpr auto select_workout_duration_sql = R"QUERY(
    SELECT
        julianday(min(ts) over ()) * 86400000.0 AS start,
        julianday(max(ts) over ()) * 86400000.0 AS end
    FROM
        measurements;
)QUERY";

constexpr auto select_current_workout_duration_sql = R"QUERY(
    SELECT
        julianday(min(ts) over ()) * 86400000.0 AS start,
        julianday('now') * 86400000.0 AS end
    FROM
        measurements;
)QUERY";


class WorkoutDataStorage {
public:
    WorkoutDataStorage();

    ~WorkoutDataStorage();

    void aggregateHeartRate(const unsigned long val);

    Aggregate getHeartRate();

    void aggregateCadence(const unsigned long val);

    Aggregate getCadence();

    void aggregateSpeed(const unsigned long val);

    Aggregate getSpeed();

    void aggregatePower(const unsigned long val);

    Aggregate getPower();

    void newWorkout();

    void startWorkout();

    void endWorkout();


    [[nodiscard]] long long getTotalWorkoutDuration() const;

    [[nodiscard]] long long getCurrentWorkoutDuration() const;

    [[nodiscard]] long long getWorkoutDuration(const std::string &query) const;

private:
    void insert(const unsigned long, const std::string &type);

    Aggregate select(const std::string &type);

private:
    static long id;
    std::unique_ptr<SQLiteConnection> connection;
    std::filesystem::path db_file;
};
