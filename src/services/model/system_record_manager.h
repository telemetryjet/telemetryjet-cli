#ifndef TELEMETRYSERVER_SYSTEM_RECORD_MANAGER_H
#define TELEMETRYSERVER_SYSTEM_RECORD_MANAGER_H

#import "model/records.h"

/**
 * System Record Manager
 * Wrapper for all functionality to interact with System objects.
 * Bridges the API endpoints and database functionality.
 *
 * This looks like it duplicates calls that could just go directly to the database,
 * but the system also manages streaming updates and stateful interactions outside
 * the database, such as creating a default system if none exists.
 * Those actions will be performed here.
 */
class SystemRecordManager {
private:
    record_system_t createDefaultSystem();
public:
    // Basic Create, Read, Update, Delete functions
                record_system_t  createSystem(std::string name);
                record_system_t  getSystem(int id);
    std::vector<record_system_t> getSystems();
                           void  updateSystem(record_system_t recordToUpdate);
                           void  deleteSystem(const record_system_t& recordToDelete);
                           void  deleteSystem(int id);

    // Get/set active system
    record_system_t getActiveSystem();
    record_system_t setActiveSystem(int id);

    // Stateful interactions with active system
    bool isSystemRunning();
    void startSystem();
    void stopSystem();
};

#endif //TELEMETRYSERVER_SYSTEM_RECORD_MANAGER_H
