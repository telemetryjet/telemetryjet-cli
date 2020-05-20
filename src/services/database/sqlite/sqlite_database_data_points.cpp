#include "sqlite_database.h"
#include <fmt/format.h>

std::vector<record_data_point_t> SqliteDatabase::getDataPoints(int system_id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    std::vector<record_data_point_t> dataPoints;
    try {
        SQLite::Statement query(*db,
                                fmt::format("select * from data_points where system_id={}",
                                            system_id));
        while (query.executeStep()) {
            dataPoints.push_back({query.getColumn(0),
                                  query.getColumn(1),
                                  query.getColumn(2),
                                  query.getColumn(3),
                                  query.getColumn(4)});
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getDataPoints: {}", e.what()));
    }
    return dataPoints;
}

record_data_point_t SqliteDatabase::getDataPoint(int id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from data_points where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()) {
        return {query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2),
                query.getColumn(3),
                query.getColumn(4)};
    } else {
        throwError(fmt::format("Error in getDataPoint: Data Point with id = {} not found.", id));
    }
}

record_data_point_t SqliteDatabase::createDataPoint(record_data_point_t dataPoint) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into data_points values (null,?,?,?,?)");
        insertStatement.bind(1, dataPoint.system_id);
        insertStatement.bind(2, dataPoint.data_frame_id);
        insertStatement.bind(3, dataPoint.timestamp);
        insertStatement.bind(4, dataPoint.data);
        insertStatement.exec();
        dataPoint.id = db->getLastInsertRowid();
        return dataPoint;
    } catch (std::exception& e) {
        throwError(fmt::format("Error in createDataPoint: {}", e.what()));
    }
}

void SqliteDatabase::updateDataPoint(record_data_point_t dataPoint) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement updateStatement(
            *db,
            "update data_points set system_id=?, data_frame_id=?, timestamp=?, data=? where id=?");
        updateStatement.bind(1, dataPoint.system_id);
        updateStatement.bind(2, dataPoint.data_frame_id);
        updateStatement.bind(3, dataPoint.timestamp);
        updateStatement.bind(4, dataPoint.data);
        updateStatement.bind(5, dataPoint.id);
        updateStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in updateDataPoint: {}", e.what()));
    }
}

void SqliteDatabase::deleteAllDataPointsForCurrentSystem() {
    deleteBySystemId("data_points", record_system_t::getActiveSystem().id);
}