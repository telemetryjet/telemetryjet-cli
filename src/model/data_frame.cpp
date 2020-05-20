#include "data_frame.h"
#include "services/service_manager.h"
#include "utility/json_utils.h"
#include "utility/time_utils.h"
#include <boost/property_tree/ptree.hpp>

record_data_frame_t record_data_frame_t::createDataFrame() {
    auto dataFrame = SM::getDatabase()->createDataFrame(
        {-1,
         record_system_t::getActiveSystem().id,
         getCurrentMillis(),
         propertyTreeToString(SM::getDataCache()->toPropertyTree())});

    SM::getStreamingServer()->sendMessageToAll(StreamingServer::OutgoingMessageType::NEW_DATA_FRAME,
                                               dataFrame.toPropertyTree());
    return dataFrame;
}

void record_data_frame_t::createDataPointsFromFrame(const record_data_frame_t& dataFrame) {
    const boost::property_tree::ptree cache = stringToPropertyTree(dataFrame.data);
    for (boost::property_tree::ptree::const_iterator it = cache.begin(); it != cache.end(); it++) {
        boost::property_tree::ptree dataPoint;
        dataPoint.add(it->first, it->second.data());
        record_data_point_t::createDataPoint(dataFrame.id,
                                             std::stoi(it->first),
                                             propertyTreeToString(dataPoint));
    }
}
