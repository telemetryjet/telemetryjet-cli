#include "sqlite_database.h"
#include <fmt/format.h>

std::vector<record_device_t> SqliteDatabase::getDevices(int system_id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    std::vector<record_device_t> devices;
    try {
        SQLite::Statement query(*db,
                                fmt::format("select * from devices where system_id={}", system_id));
        while (query.executeStep()) {
            devices.push_back(
                {query.getColumn(0), query.getColumn(1), query.getColumn(2), query.getColumn(3)});
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getDevices: {}", e.what()));
    }
    return devices;
}

record_device_t SqliteDatabase::getDevice(int id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from devices where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()) {
        return {query.getColumn(0), query.getColumn(1), query.getColumn(2), query.getColumn(3)};
    } else {
        throwError(fmt::format("Error in getDevice: Device with id = {} not found.", id));
    }
}

record_device_t SqliteDatabase::createDevice(record_device_t device) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into devices values (null,?,?,?)");
        insertStatement.bind(1, device.system_id);
        insertStatement.bind(2, device.name);
        insertStatement.bind(3, device.protocol);
        insertStatement.exec();
        device.id = db->getLastInsertRowid();
        return device;
    } catch (std::exception& e) {
        throwError(fmt::format("Error in createDevice: {}", e.what()));
    }
}

void SqliteDatabase::updateDevice(record_device_t device) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement
            updateStatement(*db, "update devices set system_id=?, name=?, protocol=? where id=?");
        updateStatement.bind(1, device.system_id);
        updateStatement.bind(2, device.name);
        updateStatement.bind(3, device.protocol);
        updateStatement.bind(4, device.id);
        updateStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in updateDevice: {}", e.what()));
    }
}