#ifndef TELEMETRYSERVER_DATA_POINT_H
#define TELEMETRYSERVER_DATA_POINT_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A Data Point
 */
struct record_data_point_t {
    int id;
    int system_id;
    int data_frame_id;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("data_frame_id", data_frame_id);
        return pt;
    }
};

#endif //TELEMETRYSERVER_DATA_POINT_H
