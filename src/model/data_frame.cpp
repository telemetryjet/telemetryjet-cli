#include "data_frame.h"
#include "services/service_manager.h"
#include "utility/json_utils.h"
#include "utility/time_utils.h"
#include <boost/property_tree/ptree.hpp>

record_data_frame_t record_data_frame_t::createDataFrame() {
    const std::vector<std::string>& keys = SM::getDataCache()->getKeys();
    const boost::property_tree::ptree cache = SM::getDataCache()->toPropertyTree();
    auto dataFrame = SM::getDatabase()->createDataFrame({-1,
                                                         record_system_t::getActiveSystem().id,
                                                         getCurrentMillis(),
                                                         propertyTreeToString(cache)});

    for (auto key : keys) {
        boost::property_tree::ptree data;
        data.add(key, cache.get_child(key).data());
        SM::getDatabase()->createDataPoint(
            {-1, record_system_t::getActiveSystem().id, dataFrame.id, propertyTreeToString(data)});
    }
    return dataFrame;
}
