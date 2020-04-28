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
    auto system = SM::getDatabase()->createSystem({-1, std::move(name)});
    SM::getLogger()
        ->info(fmt::format("Created new system: [id={}, name={}]", system.id, system.name), true);
    return system;
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
    SM::getDatabase()->updateSystem(recordToUpdate);
    SM::getLogger()->info(fmt::format("Updated system: [id={}, name={}]",
                                      recordToUpdate.id,
                                      recordToUpdate.name),
                          true);
    return recordToUpdate;
}

void record_system_t::deleteSystem(const record_system_t& recordToDelete) {
    deleteSystem(recordToDelete.id);
}

void record_system_t::deleteSystem(int id) {
    // Delete the system object
    SM::getDatabase()->deleteById("systems", id);

    // Delete all related items for system
    SM::getDatabase()->deleteAllForSystem(id);

    // Update the active system to make sure it no longer references the non-existent system
    getActiveSystem();

    SM::getLogger()->info(fmt::format("Deleted system: [id={}]", id), true);
}

record_system_t record_system_t::getActiveSystem() {
    // Get ID for active system, or -1 if active system is unset
    int activeSystemId = SM::getPersistedConfig()->getInt("activeSystem", -1);

    // Look up the active system.
    if (activeSystemId > 0) {
        try {
            return SM::getDatabase()->getSystem(activeSystemId);
        } catch (std::exception& error) {
            activeSystemId = -1;
            SM::getLogger()->warning("Active system doesn't exist");
        }
    }

    // If the active system didn't exist, find a default.
    // If there are no systems, create a default system.
    if (activeSystemId == -1) {
        const std::vector<record_system_t>& systems = SM::getDatabase()->getSystems();
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
    bool wasRunning = isSystemRunning();

    // Stop the system before we switch over
    stopSystem();

    record_system_t system = SM::getDatabase()->getSystem(id);
    SM::getPersistedConfig()->setInt("activeSystem", id);
    SM::getLogger()->info(fmt::format("Set a new active system: [id={}, name={}]",
                                      system.id,
                                      system.name),
                          true);

    if (wasRunning) {
        // Restart the system if it was previously running
        // TODO: Decide if this is actually a good idea
        // startSystem();
    }

    return system;
}

void record_system_t::startSystem() {
    SM::getPersistedConfig()->setBool("systemEnabled", true);
    SM::getDeviceManager()->start();
    SM::getLogger()->info("System activated.", true);
}

void record_system_t::stopSystem() {
    SM::getDeviceManager()->stop();
    SM::getPersistedConfig()->setBool("systemEnabled", false);
    SM::getLogger()->info("System stopped.", true);
}

bool record_system_t::isSystemRunning() {
    return SM::getPersistedConfig()->getBool("systemEnabled", true);
}