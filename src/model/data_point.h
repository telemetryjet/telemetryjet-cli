#ifndef TELEMETRYSERVER_DATA_POINT_H
#define TELEMETRYSERVER_DATA_POINT_H

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility/json_utils.h>
#include <utility/time_utils.h>

/**
 * A Data Point
 */
struct record_data_point_t {
    int id;
    int system_id;
    int data_frame_id;
    long long timestamp;
    std::string data_type;
    std::string data;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("data_frame_id", data_frame_id);
        pt.add("timestamp", timestamp);
        pt.add("data_type", data_type);
        pt.add("data", data);
        return pt;
    }

    // Basic Create, Read, Update, Delete functions
    static record_data_point_t createDataPoint(int dataFrameId,
                                               const std::string& dataType,
                                               const std::string& data,
                                               long long timestamp = getCurrentMillis());
    static std::vector<record_data_point_t>
    getDataPoints(const std::string& key, long long before, long long after);
};

#endif  // TELEMETRYSERVER_DATA_POINT_H
