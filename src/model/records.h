#ifndef TELEMETRYSERVER_RECORDS_H
#define TELEMETRYSERVER_RECORDS_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

struct record_system_t {
    int id;
    std::string name;
    static boost::property_tree::ptree toPropertyTree(const record_system_t& record) {
        boost::property_tree::ptree pt;
        pt.add("id", record.id);
        pt.add("name", record.name);
        return pt;
    }
};

/*
struct record_dashboard_t {
    int id;
    int system_id;
    std::string name;
    std::string jsonDefinition;
};

struct record_data_t {
    int id;
    int packet_id;
    long timestamp;
    std::string key;
    std::string value;
};

struct record_debug_log_t {
    int id;
    long timestamp;
    std::string message;
};
*/

#endif //TELEMETRYSERVER_RECORDS_H
