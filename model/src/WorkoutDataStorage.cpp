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

auto WorkoutDataStorage::aggregateHeartRate(const unsigned long val) const -> void {
    insert(val, "hrm");
}


auto WorkoutDataStorage::getHeartRate() const -> Aggregate {
    return select("hrm");
}

auto WorkoutDataStorage::aggregateCadence(const unsigned long val) const -> void {
    insert(val, "cad");
}

auto WorkoutDataStorage::getCadence() const -> Aggregate {
    return select("cad");
}

auto WorkoutDataStorage::aggregateSpeed(const unsigned long val) const -> void {
    insert(val, "spd");
}

auto WorkoutDataStorage::getSpeed() const -> Aggregate {
    return select("spd");
}

auto WorkoutDataStorage::aggregatePower(const unsigned long val) const -> void {
    insert(val, "pwr");
}

auto WorkoutDataStorage::getPower() const -> Aggregate {
    return select("pwr");
}

auto WorkoutDataStorage::aggregate(
    const long long timestamp,
    const unsigned long hrm,
    const unsigned long power,
    const unsigned long cadence,
    const unsigned long crankRevs,
    const unsigned long speed,
    const unsigned long wheelRevs
) const -> void {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), select_latest_sql2);
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
        prevAggregatedHrm = get(select->get(), 5);
        prevAggregatedPower = get(select->get(), 6);
        prevAggregatedCR = get(select->get(), 7);
        prevAggregatedWR = get(select->get(), 8);
    }

    auto bind = [](sqlite3_stmt *stmt, const int offset, const sqlite3_int64 value) {
        if (value == 0) {
            return sqlite3_bind_null(stmt, offset);
        }

        return sqlite3_bind_int64(stmt, offset, value);
    };

    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql2);

    rc = sqlite3_bind_int64(statement->get(), 1, timestamp);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind ts value");
    }

    rc = bind(statement->get(), 2, prev_count + 1);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `count` value");
    }

    rc = bind(statement->get(), 3, hrm);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `hrm` value");
    }

    rc = bind(statement->get(), 4, power);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `power` value");
    }

    rc = bind(statement->get(), 5, cadence);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `cadence`");
    }

    rc = bind(statement->get(), 6, speed);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `speed`");
    }

    rc = bind(statement->get(), 7, prevAggregatedHrm + hrm);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_heart_rate`");
    }
    rc = bind(statement->get(), 8, prevAggregatedPower + power);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_power`");
    }
    rc = bind(statement->get(), 9, prevAggregatedCR + crankRevs);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_crank_revs`");
    }
    rc = bind(statement->get(), 10, prevAggregatedWR + wheelRevs);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_wheel_revs`");
    }

    // TODO add count, aggregad hrm and power

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}

auto WorkoutDataStorage::newWorkout() -> void {
    const auto temp_dir = std::filesystem::temp_directory_path() / "hud";
    create_directory(temp_dir);

    const auto workout_dir = temp_dir / std::to_string(++id);
    create_directory(workout_dir);

    db_file = workout_dir / "workout.db";
    remove_all(db_file);

    connection = std::make_unique<SQLiteConnection>(db_file.string());

    char *err_msg = nullptr;
    auto rc = sqlite3_exec(connection->get(), create_tables_sql, nullptr, nullptr, &err_msg);
    if (err_msg) {
        spdlog::error("SQLite error: {}", err_msg);
    }

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to create table");
    }

    rc = sqlite3_exec(connection->get(), create_tables_sql2, nullptr, nullptr, &err_msg);
    if (err_msg) {
        spdlog::error("SQLite error: {}", err_msg);
    }

    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to create table");
    }
}

auto WorkoutDataStorage::startWorkout() const -> void {
    insert(0, "start");
}

auto WorkoutDataStorage::endWorkout() const -> void {
    insert(0, "end");
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

void WorkoutDataStorage::aggregate(
    std::chrono::duration<long long, std::ratio<1, 10000000> > timestamp,
    const unsigned long hrm,
    const unsigned long power,
    const long cadence,
    const unsigned long crankRevs,
    const long speed,
    const unsigned long wheelRevs
) {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), select_latest_sql2);
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

    auto bind = [](sqlite3_stmt *stmt, const int offset, const sqlite3_int64 value) {
        if (value == 0) {
            return sqlite3_bind_null(stmt, offset);
        }

        return sqlite3_bind_int64(stmt, offset, value);
    };

    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql2);

    rc = sqlite3_bind_int64(statement->get(), 1, timestamp.count() / 10000000 );
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind ts value");
    }

    rc = bind(statement->get(), 2, prev_count + 1);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `count` value");
    }

    rc = bind(statement->get(), 3, hrm);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `hrm` value");
    }

    rc = bind(statement->get(), 4, power);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `power` value");
    }

    rc = bind(statement->get(), 5, cadence);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `cadence`");
    }

    rc = bind(statement->get(), 6, speed);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `speed`");
    }

    rc = bind(statement->get(), 7, prevAggregatedHrm + hrm);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_heart_rate`");
    }
    rc = bind(statement->get(), 8, prevAggregatedPower + power);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_power`");
    }
    rc = bind(statement->get(), 9, prevAggregatedCR + crankRevs);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_crank_revs`");
    }
    rc = bind(statement->get(), 10, prevAggregatedWR + wheelRevs);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind `aggregated_wheel_revs`");
    }

    // TODO add count, aggregad hrm and power

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}

auto WorkoutDataStorage::insert(const unsigned long value, const std::string &type) const -> void {
    const auto select = std::make_unique<SQLiteStatement>(connection.get(), select_latest_sql);
    auto rc = sqlite3_bind_text(select->get(), 1, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind type for select");
    }

    rc = sqlite3_step(select->get());
    auto latest = 0UL;
    long count = 0UL;
    auto avg = 0.0;
    auto max = 0UL;
    auto min = 0xFFFFFFUL;

    if (rc == SQLITE_ROW) {
        latest = sqlite3_column_int(select->get(), 0);
        count = sqlite3_column_int(select->get(), 1);
        avg = sqlite3_column_double(select->get(), 2);
        max = sqlite3_column_int(select->get(), 3);
        min = sqlite3_column_int(select->get(), 4);
    }

    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), insert_sql);

    rc = sqlite3_bind_text(statement->get(), 1, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind type");
    }

    rc = sqlite3_bind_int64(statement->get(), 2, value);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind value");
    }

    rc = sqlite3_bind_int64(statement->get(), 3, count + 1);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind count");
    }

    const auto total = avg * count + value;
    rc = sqlite3_bind_double(statement->get(), 4, total / (count + 1));
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind average");
    }

    rc = sqlite3_bind_int64(statement->get(), 5, std::max(value, max));
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind max");
    }

    rc = sqlite3_bind_int64(statement->get(), 6, std::min(value, min));
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind ming");
    }

    rc = sqlite3_step(statement->get());
    if (rc != SQLITE_DONE) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute insert statement");
    }
}

auto WorkoutDataStorage::getDataAt(
    std::chrono::duration<long long, std::ratio<1, 10000000> > timestamp) const -> Aggregate2 {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_aggregate_sql2);

    const auto depth = timestamp - std::chrono::seconds(3);
    auto rc = sqlite3_bind_int64(statement->get(), 1, depth.count() / 10000000 );
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind timestamp");
    }

    rc = sqlite3_step(statement->get());
    if (rc == SQLITE_DONE) {
        return Aggregate2();
    }

    if (rc != SQLITE_ROW) {
        spdlog::error("SQLite error: {}", sqlite3_errmsg(connection->get()));
        throw std::runtime_error("Failed to execute select statement");
    }

    const auto count = sqlite3_column_int64(statement->get(), 1);
    const auto hrm = sqlite3_column_int64(statement->get(), 2);
    const auto power = sqlite3_column_int64(statement->get(), 3);
    const auto power_3s = sqlite3_column_int64(statement->get(), 4);
    const auto cadence = sqlite3_column_int64(statement->get(), 5);
    const auto speed = sqlite3_column_int64(statement->get(), 6);
    const auto hrm_avg = sqlite3_column_int64(statement->get(), 7);
    const auto power_avg = sqlite3_column_int64(statement->get(), 8);
    const auto cadence_avg = sqlite3_column_int64(statement->get(), 9);
    const auto speed_avg = sqlite3_column_int64(statement->get(), 10);


    return Aggregate2{
        static_cast<unsigned long>(hrm),
        static_cast<unsigned long>(hrm_avg) / static_cast<unsigned long>(count),
        static_cast<unsigned long>(power),
        static_cast<unsigned long>(power_avg) / static_cast<unsigned long>(count),
        static_cast<unsigned long>(power_3s),
        static_cast<unsigned long>(cadence),
        static_cast<unsigned long>(cadence_avg) / static_cast<unsigned long>(count),
        static_cast<unsigned long>(speed),
        static_cast<unsigned long>(speed_avg) / static_cast<unsigned long>(count)
    };
}

auto WorkoutDataStorage::select(const std::string &type) const -> Aggregate {
    const auto statement = std::make_unique<SQLiteStatement>(connection.get(), select_aggregate_sql);

    auto rc = sqlite3_bind_text(statement->get(), 1, type.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind type");
    }

    rc = sqlite3_step(statement->get());

    Aggregate aggregate = {};
    if (rc == SQLITE_DONE) {
        aggregate = Aggregate();
    } else if (rc == SQLITE_ROW) {
        const auto latest = sqlite3_column_int64(statement->get(), 0);
        const auto average = sqlite3_column_double(statement->get(), 1);
        const auto windowAvg = sqlite3_column_int(statement->get(), 2);
        const auto max = sqlite3_column_int(statement->get(), 3);
        const auto min = sqlite3_column_int(statement->get(), 4);

        aggregate = Aggregate{
            static_cast<unsigned long>(latest),
            static_cast<unsigned long>(std::lround(average)),
            static_cast<unsigned long>(windowAvg),
            static_cast<unsigned long>(max),
            static_cast<unsigned long>(min)

        };
    }
    return aggregate;
}
