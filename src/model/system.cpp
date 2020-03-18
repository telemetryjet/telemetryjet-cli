#include "system.h"
#include "services/service_manager.h"
#include <fmt/format.h>
#include <utility>

// Creates a default system record.
record_system_t createDefaultSystem() {
    SM::getLogger()->info("Creating a default system...");
    return record_system_t::createSystem("Default System");
}

record_system_t record_system_t::createSystem(std::string name) {
    return SM::getDatabase()->createSystem({-1, std::move(name)});
}

record_system_t record_system_t::getSystem(int id) {
    return SM::getDatabase()->getSystem(id);
}

std::vector<record_system_t> record_system_t::getSystems() {
    // Check that the default system exists before returning the list
    getActiveSystem();
    return SM::getDatabase()->getSystems();
}

record_system_t record_system_t::updateSystem(record_system_t recordToUpdate) {
    SM::getDatabase()->updateSystem(std::move(recordToUpdate));
    return recordToUpdate;
}

void record_system_t::deleteSystem(const record_system_t& recordToDelete) {
    deleteSystem(recordToDelete.id);
}

void record_system_t::deleteSystem(int id) {
    SM::getDatabase()->deleteById("systems",id);
}

record_system_t record_system_t::getActiveSystem() {
    // Get ID for active system, or -1 if active system is unset
    int activeSystemId = SM::getPersistedConfig()->getInt("activeSystem", -1);

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

record_system_t record_system_t::setActiveSystem(int id) {
    record_system_t system = SM::getDatabase()->getSystem(id);
    SM::getPersistedConfig()->setInt("activeSystem", id);
    SM::getLogger()->info(fmt::format("Set a new active system: [id={}, name={}]", system.id, system.name));
    return system;
}

void record_system_t::startSystem() {
    SM::getPersistedConfig()->setBool("systemEnabled",true);
    record_log_t::createLog("Started system!");
}

void record_system_t::stopSystem() {
    SM::getPersistedConfig()->setBool("systemEnabled",false);
    record_log_t::createLog("Stopped system!");
}

bool record_system_t::isSystemRunning() {
    return SM::getPersistedConfig()->getBool("systemEnabled",true);
}