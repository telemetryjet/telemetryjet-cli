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

    // Persisted Config Key/Values
    // Simple mechanism for persisting key and value pairs
    // virtual record_config_item_t setPersistedConfigItem(std::string key, std::string value);
    // virtual std::string getPersistedConfigItem(std::string key, std::string defaultValue);

    // Systems
    virtual std::vector<record_system_t> getSystems() = 0;
    virtual             record_system_t  getSystem(int id) = 0;
    virtual             record_system_t  createSystem(record_system_t system) = 0;
    virtual                        void  updateSystem(record_system_t system) = 0;

    // Logs
    virtual std::vector<record_log_t> getLogs(int system_id) = 0;
    virtual             record_log_t  getLog(int id) = 0;
    virtual             record_log_t  createLog(record_log_t log) = 0;
    virtual                     void  updateLog(record_log_t log) = 0;

    // Devices
    virtual std::vector<record_device_t> getDevices(int system_id) = 0;
    virtual             record_device_t  getDevice(int id) = 0;
    virtual             record_device_t  createDevice(record_device_t device) = 0;
    virtual                        void  updateDevice(record_device_t device) = 0;
};

#endif //TELEMETRYSERVER_DATABASE_H