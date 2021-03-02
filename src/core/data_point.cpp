#include "data_point.h"
#include <boost/lexical_cast.hpp>

std::shared_ptr<DataPoint> createDataPointFromString(const string_t& key, uint64_t timestamp, const string_t& stringValue) {
    if (stringValue == "true" || stringValue == "TRUE") {
        return std::make_shared<DataPoint>(key, timestamp, true);
    }
    if (stringValue == "false" || stringValue == "FALSE") {
        return std::make_shared<DataPoint>(key, timestamp, false);
    }

    // Try to interpret the string as an 64-bit float first
    try {
        float64_t convertedFloat = boost::lexical_cast<float64_t>(stringValue);
        return std::make_shared<DataPoint>(key, timestamp, convertedFloat);
    } catch (const boost::bad_lexical_cast &e) {}

    return std::make_shared<DataPoint>(key, timestamp, stringValue);
}
