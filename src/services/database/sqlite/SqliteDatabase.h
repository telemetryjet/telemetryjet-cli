#ifndef TELEMETRYSERVER_SQLITEDATABASE_H
#define TELEMETRYSERVER_SQLITEDATABASE_H

#include "services/database/Database.h"
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <SQLiteCpp/Database.h>

class SqliteDatabase : public Database {
private:
    int packet_id;
    float test{};
    bool initialized;
    SQLite::Database db;
public:
    explicit SqliteDatabase(std::string filename);
    /**
     * Raw Data.
     */
    virtual record_data_t createDataRow(long timestamp, std::string key, std::string value) override;
    virtual std::vector<record_data_t> getDataRows() override;
    virtual void startPacket() override;
    virtual void endPacket() override;

    /**
     * Dashboards.
     */
    virtual record_dashboard_t createDashboard(std::string name, std::string jsonDefinition) override;
    virtual record_dashboard_t updateDashboard(record_dashboard_t input) override;
    virtual record_dashboard_t getDashboard(int id) override;
    virtual void deleteDashboard(int id) override;
    virtual std::vector<record_dashboard_t> getDashboards() override;

    /**
     * Debug Logs.
     * Used to store various debug information
     */
    virtual record_debug_log_t createDebugLog(long timestamp, std::string message) override;
    virtual std::vector<record_debug_log_t> getDebugLogs() override;
};

#endif //TELEMETRYSERVER_SQLITEDATABASE_H
