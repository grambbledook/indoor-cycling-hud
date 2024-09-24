#pragma once
#include <sqlite3.h>
#include <sqlite_utils.h>
#include <filesystem>

#include <stdexcept>
#include <string>

#include "ModelEvents.h"

constexpr auto create_tables_sql = R"CREATE(
    CREATE TABLE measurements (
        ts TIMESTAMP,
        value INTEGER,
        count INTEGER,
        avg REAL,
        max INTEGER,
        min INTEGER,
        type VARCHAR(3),
        PRIMARY KEY (ts, type)
    );

    CREATE INDEX idx_ts_type ON measurements(ts, type) WHERE type IS NOT NULL;
)CREATE";

constexpr auto create_tables_sql2 = R"CREATE(
    CREATE TABLE measurements2 (
        ts TIMESTAMP PRIMARY KEY,
        count INTEGER,
        hrm INTEGER,
        power INTEGER,
        cadence INTEGER,
        speed INTEGER,
        aggregated_heart_rate INTEGER,
        aggregated_power INTEGER,
        aggregated_crank_revs INTEGER,
        aggregated_wheel_revs INTEGER
    );
)CREATE";

constexpr auto insert_sql = R"INSERT(
    INSERT INTO measurements (ts, type, value, count, avg, max, min)
    VALUES (datetime('now', 'subsecond'), ?, ?, ?, ?, ?, ?);
)INSERT";

constexpr auto insert_sql2 = R"INSERT(
    INSERT INTO measurements2 (ts, count, hrm, power, cadence, speed, aggregated_heart_rate, aggregated_power, aggregated_crank_revs, aggregated_wheel_revs)
    VALUES (datetime(?, 'unixepoch', 'subsecond'), ?, ?, ?, ?, ?, ?, ?, ?, ?);
)INSERT";

constexpr auto select_latest_sql2 = R"QUERY(
    SELECT
        count, aggregated_heart_rate, aggregated_power, aggregated_crank_revs, aggregated_wheel_revs
    FROM
        measurements2
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

constexpr auto select_aggregate_sql2 = R"QUERY(
    SELECT
        ts,
        count,
        hrm,
        power,
        AVG(power) OVER () AS three_sec_avg_power,
        cadence,
        speed,
        aggregated_heart_rate,
        aggregated_power,
        aggregated_crank_revs,
        aggregated_wheel_revs
    FROM
        measurements2
    WHERE
        ts >= datetime(?, 'unixepoch', 'subsecond')
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

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

    auto aggregateHeartRate(unsigned long val) const -> void;

    [[nodiscard]] auto getHeartRate() const -> Aggregate;

    auto aggregateCadence(unsigned long val) const -> void;

    [[nodiscard]] auto getCadence() const -> Aggregate;

    auto aggregateSpeed(unsigned long val) const -> void;

    [[nodiscard]] auto getSpeed() const -> Aggregate;

    auto aggregatePower(unsigned long val) const -> void;

    [[nodiscard]] auto getPower() const -> Aggregate;

    auto aggregate(
        long long timestamp,
        unsigned long hrm,
        unsigned long power,
        unsigned long cadence,
        unsigned long crankRevs,
        unsigned long speed,
        unsigned long wheelRevs
    ) const -> void;

    auto newWorkout() -> void;

    auto startWorkout() const -> void;

    auto endWorkout() const -> void;

    [[nodiscard]] auto getTotalWorkoutDuration() const -> long long;

    [[nodiscard]] auto getCurrentWorkoutDuration() const -> long long;

    [[nodiscard]] auto getWorkoutDuration(const std::string &query) const -> long long;

    [[nodiscard]] auto getDataAt(
        std::chrono::duration<long long, std::ratio<1, 10000000> > timestamp) const -> Aggregate2;

    void aggregate(
        std::chrono::duration<long long, std::ratio<1, 10000000> > timestamp,
        const unsigned long hrm,
        const unsigned long power,
        const long cadence,
        const unsigned long crankRevs,
        const long speed,
        const unsigned long wheelRevs
    );

private:
    auto insert(unsigned long, const std::string &type) const -> void;


    [[nodiscard]] auto select(const std::string &type) const -> Aggregate;

    static long id;
    std::unique_ptr<SQLiteConnection> connection;
    std::filesystem::path db_file;
};
