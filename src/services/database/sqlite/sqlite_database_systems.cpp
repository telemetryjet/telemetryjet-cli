#include <fmt/format.h>
#include "sqlite_database.h"

std::vector<record_system_t> SqliteDatabase::getSystems() {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    std::vector<record_system_t> systems;
    try {
        SQLite::Statement query(*db, "select * from systems");
        while (query.executeStep()) {
            systems.push_back({
            query.getColumn(0),
            query.getColumn(1)
            });
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getSystems: {}", e.what()));
    }
    return systems;
}

record_system_t SqliteDatabase::getSystem(int id) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from systems where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()){
        return {
            query.getColumn(0),
            query.getColumn(1)
        };
    } else {
        throwError(fmt::format("Error in getSystem: System with id = {} not found.",id));
    }
}

record_system_t SqliteDatabase::createSystem(record_system_t system) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into systems values (null,?)");
        insertStatement.bind(1, system.name);
        insertStatement.exec();
        system.id = db->getLastInsertRowid();
        return system;
    } catch (std::exception &e) {
        throwError(fmt::format("Error in createSystem: {}", e.what()));
    }
}

void SqliteDatabase::updateSystem(record_system_t system) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement updateStatement(*db, "update systems set name=? where id=?");
        updateStatement.bind(1, system.name);
        updateStatement.bind(2, system.id);
        updateStatement.exec();
    } catch (std::exception &e) {
        throwError(fmt::format("Error in updateSystem: {}", e.what()));
    }
}