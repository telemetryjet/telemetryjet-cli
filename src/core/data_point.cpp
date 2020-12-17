#include "data_point.h"

std::shared_ptr<DataPoint> createDataPointFromString(const string_t& key, uint32_t timestamp, const string_t& stringValue) {
    if (stringValue == "true" || stringValue == "TRUE") {
        return std::make_shared<DataPoint>(key, timestamp, true);
    }
    if (stringValue == "false" || stringValue == "FALSE") {
        return std::make_shared<DataPoint>(key, timestamp, false);
    }
    // Save as a string
    // TODO: Try to convert the string value to its actual data type
    return std::make_shared<DataPoint>(key, timestamp, stringValue);
}
