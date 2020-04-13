#ifndef TELEMETRYSERVER_DEVICERECORD_H
#define TELEMETRYSERVER_DEVICERECORD_H

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility/json_utils.h>

/**
 * A Device.
 */
struct record_device_t {
    int id;
    int system_id;
    std::string name;
    int protocol;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("name", name);
        pt.add("protocol", protocol);
        return pt;
    }

    // Basic Create, Read, Update, Delete functions
    static record_device_t createDevice(std::string name, int protocol);
    static record_device_t getDevice(int id);
    static std::vector<record_device_t> getDevices();
    static record_device_t updateDevice(record_device_t recordToUpdate);
    static void deleteDevice(const record_device_t& recordToDelete);
    static void deleteDevice(int id);
};

#endif  // TELEMETRYSERVER_DEVICERECORD_H