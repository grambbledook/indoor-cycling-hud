#pragma once
#include <sqlite3.h>
#include <sqlite_utils.h>
#include <filesystem>

#include <stdexcept>
#include <string>

#include "ModelEvents.h"

constexpr auto create_tables_sql = R"CREATE(
    CREATE TABLE measurements (
        ts INTEGER PRIMARY KEY,
        hrm INTEGER,
        hrm_count INTEGER,
        power INTEGER,
        power_count INTEGER,
        cadence INTEGER,
        cadence_count INTEGER,
        speed INTEGER,
        speed_count INTEGER,
        aggregated_heart_rate INTEGER,
        aggregated_power INTEGER,
        aggregated_crank_revs INTEGER,
        aggregated_wheel_revs INTEGER
    );
)CREATE";

constexpr auto insert_sql = R"INSERT(
    INSERT INTO measurements (ts, hrm, hrm_count, power, power_count, cadence, cadence_count, speed, speed_count, aggregated_heart_rate, aggregated_power, aggregated_crank_revs, aggregated_wheel_revs)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
)INSERT";

constexpr auto select_latest_sql = R"QUERY(
    SELECT
        aggregated_heart_rate, hrm_count, aggregated_power, power_count, aggregated_crank_revs, cadence_count, aggregated_wheel_revs, speed_count
    FROM
        measurements
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

constexpr auto select_window_aggregate_sql = R"QUERY(
    SELECT
        ts,
        hrm,
        hrm_count,
        power,
        power_count,
        AVG(power) OVER () AS three_sec_avg_power,
        cadence,
        cadence_count,
        speed,
        speed_count,
        aggregated_heart_rate,
        aggregated_power,
        aggregated_crank_revs,
        aggregated_wheel_revs
    FROM
        measurements
    WHERE
        ts >= ?
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";

constexpr auto select_workout_aggregate_sql = R"QUERY(
    SELECT
        AVG(hrm) AS avg_hrm,
        MIN(hrm) AS min_hrm,
        MAX(hrm) AS max_hrm,
        AVG(power) AS avg_power,
        MIN(power) AS min_power,
        MAX(power) AS max_power,
        AVG(cadence) AS avg_cadence,
        MIN(cadence) AS min_cadence,
        MAX(cadence) AS max_cadence,
        AVG(speed) AS avg_speed,
        MIN(speed) AS min_speed,
        MAX(speed) AS max_speed
    FROM
        measurements
    ORDER BY
        ts DESC
    LIMIT 1;
)QUERY";


constexpr auto select_workout_duration_sql = R"QUERY(
    SELECT
        min(ts) over () AS start,
        max(ts) over () AS end
    FROM
        measurements;
)QUERY";


class WorkoutDataStorage {
public:
    WorkoutDataStorage();

    ~WorkoutDataStorage();

    auto newWorkout() -> void;

    auto startWorkout() const -> void;

    auto endWorkout() const -> void;

    void aggregate(
        std::chrono::milliseconds timestamp,
        const unsigned long hrm,
        const unsigned long power,
        const unsigned long cadence,
        const unsigned long speed,
        const unsigned long crankRevs,
        const unsigned long wheelRevs
    ) const;

    [[nodiscard]] auto getDataAt(std::chrono::milliseconds timestamp) const -> Aggregate;

    [[nodiscard]] auto getTotalWorkoutDuration() const -> long long;

    [[nodiscard]] auto getCurrentWorkoutDuration() const -> long long;

    [[nodiscard]] auto getWorkoutSummary() const -> Aggregate;

private:
    auto signal(std::chrono::milliseconds timestamp) const -> void;

    [[nodiscard]] auto getWorkoutDuration(const std::string &query) const -> std::pair<long long, long long>;

    static long id;
    std::unique_ptr<SQLiteConnection> connection;
    std::filesystem::path db_file;
};
