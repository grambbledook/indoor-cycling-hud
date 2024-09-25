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

auto WorkoutDataStorage::startWorkout() -> void {
    const auto now = std::chrono::system_clock::now();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    signal(millis,"start");
}

auto WorkoutDataStorage::endWorkout() -> void {
    const auto now = std::chrono::system_clock::now();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    signal(millis,"end");
}

void WorkoutDataStorage::aggregate(
    std::chrono::milliseconds timestamp,
    const unsigned long hrm,
    const unsigned long power,
    const unsigned long cadence,
    const unsigned long speed,
    const unsigned long crankRevs,
    const unsigned long wheelRevs
) {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), select_latest_sql);
    auto rc = sqlite3_step(select->get());

    auto prev_count = 0UL;
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
        prev_count = get(select->get(), 0);
        prevAggregatedHrm = get(select->get(), 1);
        prevAggregatedPower = get(select->get(), 2);
        prevAggregatedCR = get(select->get(), 3);
        prevAggregatedWR = get(select->get(), 4);
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
    bind(statement->get(), prev_count + 1, "count");
    bind(statement->get(), hrm, "hrm");
    bind(statement->get(), power, "power");
    bind(statement->get(), cadence, "cadence");
    bind(statement->get(), speed, "speed");
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
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_aggregate_sql);

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

    const auto count = read(statement->get(), 1).value_or(1UL);
    const auto hrm = read(statement->get(), 2);
    const auto power = read(statement->get(), 3);
    const auto power_3s = read(statement->get(), 4);
    const auto cadence = read(statement->get(), 5);
    const auto speed = read(statement->get(), 6);
    const auto aggregatedHRM = read(statement->get(), 7);
    const auto aggregatedPOW = read(statement->get(), 8);
    const auto aggregatedCAD = read(statement->get(), 9);
    const auto aggregatedSPD = read(statement->get(), 10);

    auto avg = [](const std::optional<unsigned long> &value, const unsigned long &count) {
        return value.transform([&count](const unsigned long v) { return v / count; });
    };

    return Aggregate{
        hrm,
        avg(aggregatedHRM, count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        power,
        avg(aggregatedPOW, count),
        power_3s,
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        cadence,
        avg(aggregatedCAD, count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
        speed,
        avg(aggregatedSPD, count),
        std::optional<unsigned long>(), std::optional<unsigned long>(),
    };
}


auto WorkoutDataStorage::getTotalWorkoutDuration() const -> long long {
    return getWorkoutDuration(select_workout_duration_sql);
}

auto WorkoutDataStorage::getCurrentWorkoutDuration() const -> long long {
    return getWorkoutDuration(select_current_workout_duration_sql);
}

auto WorkoutDataStorage::getWorkoutDuration(const std::string &query) const -> long long {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), query);

    const auto rc = sqlite3_step(select->get());
    if (rc == SQLITE_ROW) {
        const auto start = sqlite3_column_int64(select->get(), 0);
        const auto end = sqlite3_column_int64(select->get(), 1);
        return end - start;
    }
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute select statement");
    }

    return 0L;
}

auto WorkoutDataStorage::signal(const std::chrono::milliseconds timestamp, const std::string &type) const -> void {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql);

    auto rc = sqlite3_bind_int64(statement->get(), 1, timestamp.count());
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind [timestamp] value");
    }

    rc = sqlite3_bind_text(statement->get(), 2, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind [type] value");
    }

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}
