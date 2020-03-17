#ifndef TELEMETRYSERVER_SQLITE_DATABASE_H
#define TELEMETRYSERVER_SQLITE_DATABASE_H

#include <vector>
#include <SQLiteCpp/Database.h>
#include "services/database/database.h"
#include <mutex>

class SqliteDatabase : public Database {
private:
    SQLite::Database *db;
    bool initialized = false;
    static void throwError(const std::string& message);
    std::mutex databaseMutex;
public:
    SqliteDatabase();
    ~SqliteDatabase() override;

    // General purpose
    void deleteById(std::string table, int id) override;
    void deleteAll(std::string table) override;

    // Config Items
    std::vector<record_config_item_t> getConfigItems() override;
                record_config_item_t  getConfigItem(std::string key) override;
                record_config_item_t  createConfigItem(record_config_item_t configItem) override;
                                void  updateConfigItem(record_config_item_t configItem) override;

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

    // Dashboards
    std::vector<record_dashboard_t> getDashboards(int system_id) override;
                record_dashboard_t  getDashboard(int id) override;
                record_dashboard_t  createDashboard(record_dashboard_t dashboard) override;
                              void  updateDashboard(record_dashboard_t dashboard) override;

    // Data Points
    std::vector<record_data_point_t> getDataPoints(int system_id) override;
                record_data_point_t  getDataPoint(int id) override;
                record_data_point_t  createDataPoint(record_data_point_t dataPoint) override;
                               void  updateDataPoint(record_data_point_t dataPoint) override;

    // Data Frames
    std::vector<record_data_frame_t> getDataFrames(int system_id) override;
                record_data_frame_t  getDataFrame(int id) override;
                record_data_frame_t  createDataFrame(record_data_frame_t dataFrame) override;
                               void  updateDataFrame(record_data_frame_t dataFrame) override;
};

#endif //TELEMETRYSERVER_SQLITE_DATABASE_H
