#ifndef DATA_POINT_H
#define DATA_POINT_H

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
    const long long timestamp;
    const float value;
    const DataPointValueType type;

    explicit DataPoint(std::string key, const float value, const long long timestamp) : key(key), timestamp(timestamp), value(value), type(DataPointValueType::FLOAT) {}
};

#endif