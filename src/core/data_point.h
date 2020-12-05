#ifndef TELEMETRYJETCLI_DATA_POINT_H
#define TELEMETRYJETCLI_DATA_POINT_H

#include <utility>
#include <variant>
#include <string>
#include <iostream>

enum class DataPointValueType : int {
    INTEGER,
    FLOAT,
    STRING
};

class DataPoint {
public:
    const std::string key;
    const float value;
    const long long timestamp;
    const DataPointValueType type;

    explicit DataPoint(std::string key, const float value, const long long timestamp) : key(key), timestamp(timestamp), value(value), type(DataPointValueType::FLOAT) {}
};

#endif //TELEMETRYJETCLI_DATA_POINT_H
