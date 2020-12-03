#ifndef TELEMETRYSERVER_DATABASE_H
#define TELEMETRYSERVER_DATABASE_H

#include "model/records.h"
#include "model/server.h"

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

    // Data Points
    virtual std::vector<record_data_point_t> getAllDataPoints(int system_id) = 0;
    virtual std::vector<record_data_point_t>
    getDataPoints(int system_id, const std::string& key, long long before, long long after) = 0;
    virtual record_data_point_t getDataPoint(int id) = 0;
    virtual record_data_point_t createDataPoint(record_data_point_t dataPoint) = 0;
    virtual void updateDataPoint(record_data_point_t dataPoint) = 0;

    // Servers
    virtual std::vector<record_server_t> getAllServers() = 0;
    virtual record_server_t getServer(const std::string& id) = 0;
    virtual bool serverExists(const std::string& id) = 0;
    virtual record_server_t createServer(record_server_t server) = 0;
    virtual void deleteServer(const std::string& id) = 0;
    virtual void deleteAllServers() = 0;
};

#endif  // TELEMETRYSERVER_DATABASE_H