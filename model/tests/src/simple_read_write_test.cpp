#include <gtest/gtest.h>
#include <sqlite3.h>
#include <filesystem>

TEST(ModelTest, SimpleReadWriteTest) {
    // Create a temporary directory for SQLite3 database
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path() / "sqlite3_test";
    std::filesystem::create_directory(temp_dir);
    std::string db_path = (temp_dir / "test.db").string();

    // Open SQLite3 database
    sqlite3* db;
    int rc = sqlite3_open(db_path.c_str(), &db);
    ASSERT_EQ(rc, SQLITE_OK);

    // Create a table
    const char* create_table_sql = "CREATE TABLE test (key TEXT PRIMARY KEY, value TEXT);";
    char* err_msg = nullptr;
    rc = sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg);
    ASSERT_EQ(rc, SQLITE_OK);
    if (err_msg) sqlite3_free(err_msg);

    // Insert a key-value pair
    const char* insert_sql = "INSERT INTO test (key, value) VALUES ('test_key', 'test_value');";
    rc = sqlite3_exec(db, insert_sql, nullptr, nullptr, &err_msg);
    ASSERT_EQ(rc, SQLITE_OK);
    if (err_msg) sqlite3_free(err_msg);

    // Read the key-value pair
    const char* select_sql = "SELECT value FROM test WHERE key = 'test_key';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr);
    ASSERT_EQ(rc, SQLITE_OK);

    rc = sqlite3_step(stmt);
    ASSERT_EQ(rc, SQLITE_ROW);
    std::string read_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    ASSERT_EQ("test_value", read_value);

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    std::filesystem::remove_all(temp_dir);
}