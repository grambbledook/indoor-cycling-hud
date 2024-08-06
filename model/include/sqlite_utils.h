#pragma once


#include <sqlite3.h>
#include <stdexcept>
#include <string>

class SQLiteConnection {
public:
    SQLiteConnection(const std::string &db_file) {
        if (sqlite3_open(db_file.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open SQLite3 database");
        }
    }

    ~SQLiteConnection() {
        if (db) {
            sqlite3_close(db);
        }
    }

    sqlite3* get() const {
        return db;
    }

private:
    sqlite3 *db = nullptr;
};

class SQLiteStatement {
public:
    SQLiteStatement(SQLiteConnection *conn, const std::string &sql) {
        if (sqlite3_prepare_v2(conn->get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }
    }

    ~SQLiteStatement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }

    sqlite3_stmt* get() const {
        return stmt;
    }

private:
    sqlite3_stmt *stmt = nullptr;
};