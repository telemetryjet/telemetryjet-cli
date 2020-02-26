#include "sqlite_database.h"
#include "services/service_manager.h"
#include <fmt/format.h>

SqliteDatabase::SqliteDatabase() {
    std::string databasePath = fmt::format("{}/database.db", ServiceManager::getConfig()->getString("data_dir","."));
    ServiceManager::getLogger()->info(fmt::format("SQLite database path: {}",databasePath));

    db = new SQLite::Database(databasePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    initialized = true;
    ServiceManager::getLogger()->info("Initialized SQLite database");
}

SqliteDatabase::~SqliteDatabase() {
    delete db;
}
