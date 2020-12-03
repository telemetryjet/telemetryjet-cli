#include "sqlite_database.h"
#include "services/service_manager.h"
#include <fmt/format.h>

void SqliteDatabase::throwError(const std::string& message) {
    ServiceManager::getLogger()->error(message);
    throw std::runtime_error(message);
}

SqliteDatabase::SqliteDatabase() {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    std::string databasePath = fmt::format("file:{}/database.db",
                                           SM::getConfig()->getString("data_dir", "."));
    ServiceManager::getLogger()->info(fmt::format("SQLite database path: {}", databasePath));

    db = new SQLite::Database(databasePath,
                              SQLite::OPEN_URI | SQLite::OPEN_READWRITE
                                  | SQLite::OPEN_CREATE);  // NOLINT(hicpp-signed-bitwise)

    ServiceManager::getLogger()->info("Checking migrations...");

    // Create tables if they do not already exist.
    db->exec(
        "create table if not exists data_points (id integer primary key, system_id integer, "
        "data_frame_id integer, timestamp text not null, data_type text not null, data text not "
        "null)");
    db->exec(
        "create table if not exists servers (id text primary key, host text not null, username "
        "text not null, password text not null)");

    ServiceManager::getLogger()->info("Started SQLite database.");
    initialized = true;
}

SqliteDatabase::~SqliteDatabase() {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    delete db;
    ServiceManager::getLogger()->info("Stopped SQLite database.");
}

void SqliteDatabase::deleteById(std::string table, int id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement deleteStatement(*db, fmt::format("delete from {} where id=?", table));
        deleteStatement.bind(1, id);
        deleteStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in deleteById: {}", e.what()));
    }
}

void SqliteDatabase::deleteAll(std::string table) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement deleteStatement(*db, fmt::format("truncate table {}", table));
        deleteStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in deleteAll: {}", e.what()));
    }
}