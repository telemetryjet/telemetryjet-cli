#include "data_point.h"
#include "services/service_manager.h"
#include "utility/time_utils.h"

record_data_point_t
record_data_point_t::createDataPoint(int dataFrameId, int dataType, std::string data) {
    return SM::getDatabase()->createDataPoint({-1,
                                               record_system_t::getActiveSystem().id,
                                               dataFrameId,
                                               getCurrentMillis(),
                                               dataType,
                                               data});
}
