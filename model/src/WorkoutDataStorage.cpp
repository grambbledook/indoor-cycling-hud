#include "WorkoutDataStorage.h"

#include <cassert>
#include <sqlite_utils.h>
#include <filesystem>
#include <chrono>
#include <spdlog/spdlog.h>

long WorkoutDataStorage::id = 0L;

WorkoutDataStorage::WorkoutDataStorage() {
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

WorkoutDataStorage::~WorkoutDataStorage() {
}

void WorkoutDataStorage::aggregateHeartRate(const long long ts, const long val) {
    insert(ts, val, "hrm");
}

Aggregate WorkoutDataStorage::getHeartRate() {
    return select("hrm");
}

void WorkoutDataStorage::aggregateCadence(const long long ts, const long val) {
    insert(ts, val, "cad");
}

Aggregate WorkoutDataStorage::getCadence() {
    return select("cad");
}

void WorkoutDataStorage::aggregateSpeed(const long long ts, const long val) {
    insert(ts, val, "spd");
}

Aggregate WorkoutDataStorage::getSpeed() {
    return select("spd");
}

void WorkoutDataStorage::aggregatePower(const long long ts, const long val) {
    insert(ts, val, "pwr");
}

Aggregate WorkoutDataStorage::getPower() {
    return select("pwr");
}

void WorkoutDataStorage::insert(long long ts, long value, const std::string &type) {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql);

    auto rc = sqlite3_bind_int64(statement->get(), 1, ts);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind timestamp");
    }

    rc = sqlite3_bind_int64(statement->get(), 2, value);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind measurement");
    }

    rc = sqlite3_bind_text(statement->get(), 3, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind type");
    }

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute insert statement");
    }
}

Aggregate WorkoutDataStorage::select(const std::string &type) {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_sql);

    auto rc = sqlite3_bind_text(statement->get(), 1, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind type");
    }

    rc = sqlite3_step(statement->get());

    Aggregate aggregate = {};
    if (rc == SQLITE_DONE) {
        aggregate = Aggregate();
    } else if (rc == SQLITE_ROW) {
        const auto latest = sqlite3_column_int(statement->get(), 0);
        const auto average = sqlite3_column_int(statement->get(), 1);

        aggregate = Aggregate{
            latest,
            average,
        };
    }
    return aggregate;
}