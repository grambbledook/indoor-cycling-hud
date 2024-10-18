#include "WorkoutDataStorage.h"

#include <cassert>
#include <sqlite_utils.h>
#include <filesystem>
#include <chrono>
#include <spdlog/spdlog.h>

long WorkoutDataStorage::id = 0L;

WorkoutDataStorage::WorkoutDataStorage() {
    newWorkout();
}

WorkoutDataStorage::~WorkoutDataStorage() = default;

auto WorkoutDataStorage::newWorkout() -> void {
    const auto temp_dir = std::filesystem::temp_directory_path() / "hud";
    create_directory(temp_dir);

    const auto workout_dir = temp_dir / std::to_string(++id);
    create_directory(workout_dir);

    db_file = workout_dir / "workout.db";
    remove_all(db_file);

    connection = std::make_unique<SQLiteConnection>(db_file.string());

    char *err_msg = nullptr;
    const auto rc = sqlite3_exec(connection->get(), create_tables_sql, nullptr, nullptr, &err_msg);
    if (err_msg) {
        spdlog::error("SQLite error: {}", err_msg);
    }

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to create table");
    }
}

auto WorkoutDataStorage::startWorkout() const -> void {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    signal(timestamp);
}

auto WorkoutDataStorage::endWorkout() const -> void {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    signal(timestamp);
}

void WorkoutDataStorage::aggregate(
    const std::chrono::milliseconds timestamp,
    const unsigned long hrm,
    const unsigned long power,
    const unsigned long cadence,
    const unsigned long speed,
    const unsigned long crankRevs,
    const unsigned long wheelRevs
) const {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), select_latest_sql);
    auto rc = sqlite3_step(select->get());

    auto hrm_count = 0UL;
    auto power_count = 0UL;
    auto speed_count = 0UL;
    auto cadence_count = 0UL;
    auto prevAggregatedHrm = 0UL;
    auto prevAggregatedPower = 0UL;
    auto prevAggregatedCR = 0UL;
    auto prevAggregatedWR = 0UL;

    auto get = [](sqlite3_stmt *stmt, const int offset) {
        return sqlite3_column_type(stmt, offset) == SQLITE_NULL
                   ? 0UL
                   : static_cast<unsigned long>(sqlite3_column_int64(stmt, offset));
    };

    if (rc == SQLITE_ROW) {
        prevAggregatedHrm = get(select->get(), 0);
        hrm_count = get(select->get(), 1);
        prevAggregatedPower = get(select->get(), 2);
        power_count = get(select->get(), 3);
        prevAggregatedCR = get(select->get(), 4);
        cadence_count = get(select->get(), 5);
        prevAggregatedWR = get(select->get(), 6);
        speed_count = get(select->get(), 7);
    }

    auto offset = 1;
    auto bind = [&offset](sqlite3_stmt *stmt, const sqlite3_int64 value, std::string name) {
        const auto rc = (value == 0) ? sqlite3_bind_null(stmt, offset) : sqlite3_bind_int64(stmt, offset, value);
        offset++;
        if (rc != SQLITE_OK) {
            throw std::runtime_error(fmt::format("Failed to bind [%s] value", name));
        }
    };

    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql);

    auto duration = timestamp.count();

    bind(statement->get(), duration, "ts");
    bind(statement->get(), hrm, "hrm");
    bind(statement->get(), hrm_count + (hrm == 0 ? 0 : 1), "hrm_count");
    bind(statement->get(), power, "power");
    bind(statement->get(), power_count + (power == 0 ? 0 : 1), "power_count");
    bind(statement->get(), cadence, "cadence");
    bind(statement->get(), cadence_count + (cadence == 0 ? 0 : 1), "cadence_count");
    bind(statement->get(), speed, "speed");
    bind(statement->get(), speed_count + (speed == 0 ? 0 : 1), "speed_count");
    bind(statement->get(), prevAggregatedHrm + hrm, "aggregated_heart_rate");
    bind(statement->get(), prevAggregatedPower + power, "aggregated_power");
    bind(statement->get(), prevAggregatedCR + crankRevs, "aggregated_crank_revs");
    bind(statement->get(), prevAggregatedWR + wheelRevs, "aggregated_wheel_revs");

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}

auto WorkoutDataStorage::getDataAt(const std::chrono::milliseconds timestamp) const -> Aggregate {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_window_aggregate_sql);

    const auto depth = timestamp - std::chrono::milliseconds(3000);
    auto rc = sqlite3_bind_int64(statement->get(), 1, depth.count());
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind timestamp");
    }

    rc = sqlite3_step(statement->get());
    if (rc == SQLITE_DONE) {
        return Aggregate();
    }

    if (rc != SQLITE_ROW) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute select statement");
    }

    auto read = [](sqlite3_stmt *stmt, const int offset) {
        if (sqlite3_column_type(stmt, offset) == SQLITE_NULL) {
            return std::optional<unsigned long>();
        }

        const auto value = sqlite3_column_int64(stmt, offset);
        return std::optional(static_cast<unsigned long>(value));
    };

    const auto hrm = read(statement->get(), 1);
    const auto hrm_count = read(statement->get(), 2).value_or(1UL);
    const auto power = read(statement->get(), 3);
    const auto power_count = read(statement->get(), 4).value_or(1UL);
    const auto power_3s = read(statement->get(), 5);
    const auto cadence = read(statement->get(), 6);
    const auto cadence_count = read(statement->get(), 7).value_or(1UL);
    const auto speed = read(statement->get(), 8);
    const auto speed_count = read(statement->get(), 9).value_or(1UL);
    const auto aggregatedHRM = read(statement->get(), 10);
    const auto aggregatedPOW = read(statement->get(), 11);
    const auto aggregatedCAD = read(statement->get(), 12);
    const auto aggregatedSPD = read(statement->get(), 13);

    auto avg = [](const std::optional<unsigned long> &value, const unsigned long &count) {
        return value.transform([&count](const unsigned long v) { return v / count; });
    };

    return Aggregate{
        hrm,
        avg(aggregatedHRM, hrm_count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        power,
        power_3s,
        avg(aggregatedPOW, power_count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        cadence,
        avg(aggregatedCAD, cadence_count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        speed,
        avg(aggregatedSPD, speed_count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
    };
}


auto WorkoutDataStorage::getTotalWorkoutDuration() const -> long long {
    auto [start, end] = getWorkoutDuration(select_workout_duration_sql);

    return end - start;
}

auto WorkoutDataStorage::getCurrentWorkoutDuration() const -> long long {
    const auto now = std::chrono::system_clock::now();
    const auto end = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    auto [start, _] = getWorkoutDuration(select_workout_duration_sql);
    return end.count() - start;
}

auto WorkoutDataStorage::getWorkoutSummary() const -> Aggregate {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_workout_aggregate_sql);

    const auto rc = sqlite3_step(statement->get());
    if (rc == SQLITE_DONE) {
        return Aggregate();
    }

    if (rc != SQLITE_ROW) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute select statement");
    }

    auto read = [](sqlite3_stmt *stmt, const int offset) {
        if (sqlite3_column_type(stmt, offset) == SQLITE_NULL) {
            return std::optional<unsigned long>();
        }

        const auto value = sqlite3_column_int64(stmt, offset);
        return std::optional(static_cast<unsigned long>(value));
    };

    const auto avg_hrm = read(statement->get(), 0);
    const auto min_hrm = read(statement->get(), 1);
    const auto max_hrm = read(statement->get(), 2);

    const auto avg_power = read(statement->get(), 3);
    const auto min_power = read(statement->get(), 4);
    const auto max_power = read(statement->get(), 5);

    const auto avg_cadence = read(statement->get(), 6);
    const auto min_cadence = read(statement->get(), 7);
    const auto max_cadence = read(statement->get(), 8);

    const auto avg_speed = read(statement->get(), 9);
    const auto min_speed = read(statement->get(), 10);
    const auto max_speeed = read(statement->get(), 11);

    return Aggregate{
        std::optional<unsigned long>(),
        avg_hrm, min_hrm, max_hrm,
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        avg_power, min_power, max_power,
        std::optional<unsigned long>(),
        avg_cadence, min_cadence, max_cadence,
        std::optional<unsigned long>(),
        avg_speed, min_speed, max_speeed,
    };
}

auto WorkoutDataStorage::getWorkoutDuration(const std::string &query) const -> std::pair<long long, long long> {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), query);

    const auto rc = sqlite3_step(select->get());
    if (rc == SQLITE_ROW) {
        const auto start = sqlite3_column_int64(select->get(), 0);
        const auto end = sqlite3_column_int64(select->get(), 1);
        return {start, end};
    }
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute select statement");
    }

    return {0L, 0L};
}

auto WorkoutDataStorage::signal(const std::chrono::milliseconds timestamp) const -> void {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql);

    auto rc = sqlite3_bind_int64(statement->get(), 1, timestamp.count());
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind [timestamp] value");
    }

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}
