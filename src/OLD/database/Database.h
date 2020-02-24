#ifndef TELEMETRYSERVER_DATABASE_H
#define TELEMETRYSERVER_DATABASE_H

#include <data/recordTypes.h>

/**
 * Database
 * Interface for saving/loading stored data from an outside source
 * Used for saving configuration and data point history
 */
class Database {
public:
    /**
     * Raw Data.
     */
    virtual record_data_t createDataRow(long timestamp, std::string key, std::string value) = 0;
    virtual std::vector<record_data_t> getDataRows() = 0;
    virtual void startPacket() = 0;
    virtual void endPacket() = 0;

    /**
     * Dashboards.
     */
    virtual record_dashboard_t createDashboard(std::string name, std::string jsonDefinition) = 0;
    virtual record_dashboard_t updateDashboard(record_dashboard_t input) = 0;
    virtual record_dashboard_t getDashboard(int id) = 0;
    virtual void deleteDashboard(int id) = 0;
    virtual std::vector<record_dashboard_t> getDashboards() = 0;

    /**
     * Debug Logs.
     * Used to store various debug information
     */
    virtual record_debug_log_t createDebugLog(long timestamp, std::string message) = 0;
    virtual std::vector<record_debug_log_t> getDebugLogs() = 0;
};

#endif //TELEMETRYSERVER_SQLITEDATABASE_H
