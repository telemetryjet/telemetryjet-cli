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

    // Systems
    virtual std::vector<record_system_t> getSystems() = 0;
    virtual             record_system_t  getSystem(int id) = 0;
    virtual             record_system_t  createSystem(record_system_t system) = 0;
    virtual                        void  updateSystem(record_system_t system) = 0;
    virtual                        void  deleteSystem(int id) = 0;
};

#endif //TELEMETRYSERVER_DATABASE_H