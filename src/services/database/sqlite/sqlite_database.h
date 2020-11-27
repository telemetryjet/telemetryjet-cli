#ifndef TELEMETRYSERVER_SQLITE_DATABASE_H
#define TELEMETRYSERVER_SQLITE_DATABASE_H

#include "services/database/database.h"
#include <SQLiteCpp/Database.h>
#include <mutex>
#include <vector>

class SqliteDatabase : public Database {
private:
    SQLite::Database* db;
    bool initialized = false;
    static void throwError(const std::string& message);
    std::mutex databaseMutex;

public:
    SqliteDatabase();
    ~SqliteDatabase() override;

    // General purpose
    void deleteById(std::string table, int id) override;
    void deleteAll(std::string table) override;

    // Data Points
    std::vector<record_data_point_t> getAllDataPoints(int system_id) override;
    std::vector<record_data_point_t>
    getDataPoints(int system_id, const std::string& key, long long before, long long after) override;
    record_data_point_t getDataPoint(int id) override;
    record_data_point_t createDataPoint(record_data_point_t dataPoint) override;
    void updateDataPoint(record_data_point_t dataPoint) override;
    void deleteAllDataPointsForCurrentSystem() override;
};

#endif  // TELEMETRYSERVER_SQLITE_DATABASE_H
