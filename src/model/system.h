#ifndef TELEMETRYSERVER_SYSTEM_H
#define TELEMETRYSERVER_SYSTEM_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A System.
 */
struct record_system_t {
    int id;
    std::string name;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("name", name);
        return pt;
    }

    // Basic Create, Read, Update, Delete functions
    static             record_system_t  createSystem(std::string name);
    static             record_system_t  getSystem(int id);
    static std::vector<record_system_t> getSystems();
    static             record_system_t  updateSystem(record_system_t recordToUpdate);
    static                        void  deleteSystem(const record_system_t& recordToDelete);
    static                        void  deleteSystem(int id);

    // Get/set active system
    static record_system_t getActiveSystem();
    static record_system_t setActiveSystem(int id);

    // Stateful interactions with active system
    static bool isSystemRunning();
    static void startSystem();
    static void stopSystem();
};

#endif //TELEMETRYSERVER_SYSTEM_H