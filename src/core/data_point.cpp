#include "data_point.h"
#include "services/service_manager.h"

record_data_point_t record_data_point_t::createDataPoint(int dataFrameId,
                                                         const std::string& dataType,
                                                         const std::string& data,
                                                         long long timestamp) {
    return SM::getDatabase()->createDataPoint(
        {-1, record_system_t::getActiveSystem().id, dataFrameId, timestamp, dataType, data});
}

std::vector<record_data_point_t>
record_data_point_t::getDataPoints(const std::string& key, long long int before, long long int after) {
    return SM::getDatabase()->getDataPoints(record_system_t::getActiveSystem().id,
                                            key,
                                            before,
                                            after);
}
