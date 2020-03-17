#include "system_record_manager.h"
#include "services/service_manager.h"
#include <fmt/format.h>

// Creates a default system record.
record_system_t SystemRecordManager::createDefaultSystem() {
    SM::getLogger()->info("Creating a default system...");
    return createSystem("Default System");
}

record_system_t SystemRecordManager::createSystem(std::string name) {
    return SM::getDatabase()->createSystem({
        -1,
        name
    });
}

record_system_t SystemRecordManager::getSystem(int id) {
    return SM::getDatabase()->getSystem(id);
}

std::vector<record_system_t> SystemRecordManager::getSystems() {
    return SM::getDatabase()->getSystems();
}

void SystemRecordManager::updateSystem(record_system_t recordToUpdate) {
    SM::getDatabase()->updateSystem(recordToUpdate);
}

void SystemRecordManager::deleteSystem(const record_system_t& recordToDelete) {
    deleteSystem(recordToDelete.id);
}

void SystemRecordManager::deleteSystem(int id) {
    SM::getDatabase()->deleteById("systems",id);
}

record_system_t SystemRecordManager::getActiveSystem() {
    // Get ID for active system, or -1 if active system is unset
    int activeSystemId = SM::getConfig()->getInt("activeSystem", -1);

    // Look up the active system.
    if (activeSystemId > 0) {
        try {
            return SM::getDatabase()->getSystem(activeSystemId);
        } catch (std::exception &error){
            activeSystemId = -1;
            SM::getLogger()->warning("Active system doesn't exist");
        }
    }

    // If the active system didn't exist, find a default.
    // If there are no systems, create a default system.
    if (activeSystemId == -1) {
        const std::vector<record_system_t> &systems = SM::getDatabase()->getSystems();
        record_system_t activeSystem;
        if (!systems.empty()) {
            activeSystem = systems.front();
        } else {
            activeSystem = createDefaultSystem();
        }
        // Set the active system
        return setActiveSystem(activeSystem.id);
    }
}

record_system_t SystemRecordManager::setActiveSystem(int id) {
    record_system_t system = SM::getDatabase()->getSystem(id);
    SM::getConfig()->setInt("activeSystem", id);
    SM::getLogger()->info(fmt::format("Set a new active system: [id={}, name={}]", system.id, system.name));
    return system;
}

void SystemRecordManager::startSystem() {
    SM::getConfig()->setBool("systemEnabled",true);
}

void SystemRecordManager::stopSystem() {
    SM::getConfig()->setBool("systemEnabled",false);
}

bool SystemRecordManager::isSystemRunning() {
    SM::getConfig()->getBool("systemEnabled",true);
}