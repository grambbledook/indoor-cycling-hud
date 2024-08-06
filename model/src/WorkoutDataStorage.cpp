#include "WorkoutDataStorage.h"

#include <cassert>
#include <sqlite3.h>
#include <filesystem>
#include <chrono>

long WorkoutDataStorage::id = 0L;

WorkoutDataStorage::WorkoutDataStorage() {
    const auto temp_dir = std::filesystem::temp_directory_path() / "hud" ;
    create_directory(temp_dir);

    const auto workout_dir = temp_dir / std::to_string(++id);
    create_directory(workout_dir);

    db_file = workout_dir / "workout.db";
    remove_all(db_file);

    sqlite3 *db;
    int rc = sqlite3_open(db_file.string().c_str(), &db);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to open SQLite3 database");
    }

    connection = std::unique_ptr<sqlite3, sqlite3_deleter>(db, sqlite3_deleter());

    const auto create_table_sql = R"CREATE(
    CREATE TABLE measurements (
        ts INTEGER PRIMARY KEY,
        value INTEGER,
        type VARCHAR(3)
    );

    CREATE INDEX idx_ts_type ON measurements(ts, type) WHERE type IS NOT NULL;)CREATE";


    char *err_msg = nullptr;
    rc = sqlite3_exec(connection.get(), create_table_sql, nullptr, nullptr, &err_msg);
    if (err_msg) {
        std::cout << "BLAHA " << err_msg << std::endl;
    }

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to Create table");
    }
}
WorkoutDataStorage::~WorkoutDataStorage() {
    sqlite3_close(connection.get());

    remove_all(db_file);
}

void WorkoutDataStorage::aggregateHeartRate(long long timestamp, long measurement) {
    const auto insert_sql = "INSERT INTO measurements (ts, value, type) VALUES (?, ?, 'hrm');";

    char *err_msg = nullptr;
    const auto rc = sqlite3_exec(connection.get(), insert_sql, nullptr, nullptr, &err_msg);
    if (err_msg) {
        std::cout << "BLAHA insert " << err_msg << std::endl;
    }

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to Create table");
    }
}

Aggregate WorkoutDataStorage::getHeartRate() {
    const auto select_sql = R"QUERY(SELECT
                                        value,
                                        AVG(value) AS avg_value,
                                        MIN(value) AS min_value,
                                        MAX(value) AS max_value
                                    FROM measurements
                                    WHERE type = 'hrm'
                                    GROUP BY type;
                            )QUERY";

    sqlite3_stmt *stmt;
    const auto rc = sqlite3_prepare_v2(connection.get(), select_sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to select data");
    }

    return Aggregate{
        sqlite3_column_int(stmt, 0),
        sqlite3_column_int(stmt, 1),
    };
}

void WorkoutDataStorage::aggregateCadence(long long timestamp, long measurement) {
}

Aggregate WorkoutDataStorage::getCadence() {
    return Aggregate();
}

void WorkoutDataStorage::aggregateSpeed(long long timestamp, long measurement) {
}

Aggregate WorkoutDataStorage::getSpeed() {
    return Aggregate();
}

void WorkoutDataStorage::aggregatePower(long long timestamp, long measurement) {
}

Aggregate WorkoutDataStorage::getPower() {
    return Aggregate();
}
