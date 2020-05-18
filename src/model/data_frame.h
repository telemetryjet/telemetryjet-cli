#ifndef TELEMETRYSERVER_DATA_FRAME_H
#define TELEMETRYSERVER_DATA_FRAME_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A Data Frame.
 */
struct record_data_frame_t {
    int id;
    int system_id;
    uint64_t timestamp;
    std::string data;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("data", data);
        return pt;
    }
};

#endif //TELEMETRYSERVER_DATA_FRAME_H
