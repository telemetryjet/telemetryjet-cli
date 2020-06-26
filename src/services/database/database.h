#ifndef TELEMETRYSERVER_DATABASE_H
#define TELEMETRYSERVER_DATABASE_H

#include "model/records.h"

/**
 * Database
 * Handles storage of persistant data,
 * such as system configuration and data points.
 */
class Database {
public:
    virtual ~Database() = default;

    // General purpose
    virtual void deleteById(std::string table, int id) = 0;
    virtual void deleteAll(std::string table) = 0;
    virtual void deleteBySystemId(std::string table, int system_id) = 0;
    virtual void deleteAllForSystem(int system_id) = 0;

    // Config Items
    virtual std::vector<record_config_item_t> getConfigItems() = 0;
    virtual record_config_item_t getConfigItem(std::string key) = 0;
    virtual record_config_item_t createConfigItem(record_config_item_t configItem) = 0;
    virtual void updateConfigItem(record_config_item_t configItem) = 0;

    // Systems
    virtual std::vector<record_system_t> getSystems() = 0;
    virtual record_system_t getSystem(int id) = 0;
    virtual record_system_t createSystem(record_system_t system) = 0;
    virtual void updateSystem(record_system_t system) = 0;

    // Logs
    virtual std::vector<record_log_t> getLogs(int system_id) = 0;
    virtual std::vector<record_log_t> getRecentLogs(int system_id, int limit, int beforeId) = 0;
    virtual record_log_t getLog(int id) = 0;
    virtual record_log_t createLog(record_log_t log) = 0;
    virtual void updateLog(record_log_t log) = 0;

    // Devices
    virtual std::vector<record_device_t> getDevices(int system_id) = 0;
    virtual record_device_t getDevice(int id) = 0;
    virtual record_device_t createDevice(record_device_t device) = 0;
    virtual void updateDevice(record_device_t device) = 0;

    // Dashboards
    virtual std::vector<record_dashboard_t> getDashboards(int system_id) = 0;
    virtual record_dashboard_t getDashboard(int id) = 0;
    virtual record_dashboard_t createDashboard(record_dashboard_t dashboard) = 0;
    virtual void updateDashboard(record_dashboard_t dashboard) = 0;

    // Data Points
    virtual std::vector<record_data_point_t> getAllDataPoints(int system_id) = 0;
    virtual std::vector<record_data_point_t>
    getDataPoints(int system_id, const std::string& key, long long before, long long after) = 0;
    virtual record_data_point_t getDataPoint(int id) = 0;
    virtual record_data_point_t createDataPoint(record_data_point_t dataPoint) = 0;
    virtual void updateDataPoint(record_data_point_t dataPoint) = 0;
    virtual void deleteAllDataPointsForCurrentSystem() = 0;

    // Data Frames
    virtual std::vector<record_data_frame_t> getAllDataFrames(int system_id) = 0;
    virtual std::vector<record_data_frame_t>
    getDataFrames(int system_id, long long before, long long after) = 0;
    virtual record_data_frame_t getDataFrame(int id) = 0;
    virtual record_data_frame_t createDataFrame(record_data_frame_t dataFrame) = 0;
    virtual void updateDataFrame(record_data_frame_t dataFrame) = 0;
    virtual void deleteAllDataFramesForCurrentSystem() = 0;
};

#endif  // TELEMETRYSERVER_DATABASE_H