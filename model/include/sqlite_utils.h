#pragma once


#include <sqlite3.h>
#include <stdexcept>
#include <string>

class SQLiteConnection {
public:
    explicit SQLiteConnection(const std::string &db_file) {
        if (sqlite3_open(db_file.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open SQLite3 database");
        }
    }

    ~SQLiteConnection() {
        if (db) {
            sqlite3_close(db);
        }
    }

    [[nodiscard]] auto get() const -> sqlite3* {
        return db;
    }

private:
    sqlite3 *db = nullptr;
};

class SQLiteStatement {
public:
    SQLiteStatement(const SQLiteConnection *conn, const std::string &sql) {
        auto rc = sqlite3_prepare_v2(conn->get(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement" + std::string(sqlite3_errmsg(conn->get())));
        }
    }

    ~SQLiteStatement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }

    [[nodiscard]] auto get() const -> sqlite3_stmt* {
        return stmt;
    }

private:
    sqlite3_stmt *stmt = nullptr;
};