#ifndef TELEMETRYSERVER_SQLITE_DATABASE_H
#define TELEMETRYSERVER_SQLITE_DATABASE_H

#include <vector>
#include <SQLiteCpp/Database.h>
#include "services/database/database.h"

class SqliteDatabase : public Database {
private:
    SQLite::Database *db;
    bool initialized = false;
    static void throwError(std::string message);
public:
    SqliteDatabase();
    ~SqliteDatabase() override;

    // General purpose
    void deleteById(std::string table, int id) override;
    void deleteAll(std::string table) override;

    // Systems
    std::vector<record_system_t> getSystems() override;
                record_system_t  getSystem(int id) override;
                record_system_t  createSystem(record_system_t system) override;
                           void  updateSystem(record_system_t system) override;

    // Logs
    std::vector<record_log_t> getLogs(int system_id) override;
                record_log_t  getLog(int id) override;
                record_log_t  createLog(record_log_t log) override;
                        void  updateLog(record_log_t log) override;

    // Devices
    std::vector<record_device_t> getDevices(int system_id) override;
                record_device_t  getDevice(int id) override;
                record_device_t  createDevice(record_device_t device) override;
                           void  updateDevice(record_device_t device) override;
};

#endif //TELEMETRYSERVER_SQLITE_DATABASE_H
