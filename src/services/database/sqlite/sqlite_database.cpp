#include "sqlite_database.h"
#include "services/service_manager.h"
#include <fmt/format.h>

SqliteDatabase::SqliteDatabase() {
    std::string databasePath = fmt::format("{}/database.db", ServiceManager::getConfig()->getString("data_dir","."));
    ServiceManager::getLogger()->info(fmt::format("SQLite database path: {}",databasePath));

    db = new SQLite::Database(databasePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE); // NOLINT(hicpp-signed-bitwise)

    ServiceManager::getLogger()->info("Checking migrations...");

    // Create tables if they do not already exist.
    db->exec("create table if not exists systems (id integer primary key, name text not null)");

    ServiceManager::getLogger()->info("Initialized SQLite database");
    initialized = true;
}

SqliteDatabase::~SqliteDatabase() {
    delete db;
}
