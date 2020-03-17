#include "sqlite_database.h"
#include "services/service_manager.h"
#include <fmt/format.h>

void SqliteDatabase::throwError(std::string message){
    ServiceManager::getLogger()->error(message);
    throw std::runtime_error(message);
}

SqliteDatabase::SqliteDatabase() {
    std::string databasePath = fmt::format("{}/database.db", ServiceManager::getConfig()->getString("data_dir","."));
    ServiceManager::getLogger()->info(fmt::format("SQLite database path: {}",databasePath));

    db = new SQLite::Database(databasePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE); // NOLINT(hicpp-signed-bitwise)

    ServiceManager::getLogger()->info("Checking migrations...");

    // Create tables if they do not already exist.
    db->exec("create table if not exists systems (id integer primary key, name text not null)");
    db->exec("create table if not exists logs (id integer primary key, system_id integer, message text not null)");
    db->exec("create table if not exists devices (id integer primary key, system_id integer, name text not null)");

    ServiceManager::getLogger()->info("Initialized SQLite database");
    initialized = true;
}

SqliteDatabase::~SqliteDatabase() {
    delete db;
}

void SqliteDatabase::deleteById(std::string table, int id) {
    try {
        SQLite::Statement deleteStatement(*db, fmt::format("delete from {} where id=?",table));
        deleteStatement.bind(1, id);
        deleteStatement.exec();
    } catch (std::exception &e) {
        throwError(fmt::format("Error in deleteById: {}", e.what()));
    }
}

void SqliteDatabase::deleteAll(std::string table) {
    try {
        SQLite::Statement deleteStatement(*db, fmt::format("truncate table {}",table));
        deleteStatement.exec();
    } catch (std::exception &e) {
        throwError(fmt::format("Error in deleteAll: {}", e.what()));
    }
}