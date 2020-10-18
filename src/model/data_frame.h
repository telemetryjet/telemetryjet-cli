#ifndef TELEMETRYSERVER_DATA_FRAME_H
#define TELEMETRYSERVER_DATA_FRAME_H

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <sstream>
#include <utility/json_utils.h>

/**
 * A Data Frame.
 */
struct record_data_frame_t {
    int id;
    int system_id;
    long long timestamp;
    std::string data;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("timestamp", timestamp);
        // Convert the string data back into a JSON object.
        std::stringstream dataStream;
        dataStream << data;
        boost::property_tree::ptree dataPt;
        boost::property_tree::read_json(dataStream, dataPt);
        pt.add_child("data", dataPt);
        return pt;
    }

    static void createDataPointsFromFrame(const record_data_frame_t& dataFrame);

    // Basic Create, Read, Update, Delete functions
    static record_data_frame_t createDataFrame();
    static std::vector<record_data_frame_t> getDataFrames(long long before, long long after);
};

#endif  // TELEMETRYSERVER_DATA_FRAME_H
