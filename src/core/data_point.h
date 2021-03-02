#ifndef DATA_POINT_H
#define DATA_POINT_H

#include <boost/variant.hpp>
#include <fmt/format.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <variant>
#include "utility/messagepack/messagepack.h"

using json = nlohmann::json;

// Alias various types so we acn easily change their underlying type if needed
typedef std::string string_t;
typedef bool bool_t;
typedef float float32_t;
typedef double float64_t;
typedef boost::variant<string_t,
                       bool_t,
                       uint8_t,
                       uint16_t,
                       uint32_t,
                       uint64_t,
                       int8_t,
                       int16_t,
                       int32_t,
                       int64_t,
                       float32_t,
                       float64_t>
    data_type_union_t;

enum class DataPointType : int {
    EVENT,
    STRING,
    BOOLEAN,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64
};

class DataPoint {
public:
    string_t key;
    const uint64_t timestamp;
    const data_type_union_t value;
    const DataPointType type;

    DataPoint(string_t key, uint64_t timestamp)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(true)
        , type(DataPointType::EVENT) {
    }
    DataPoint(string_t key, uint64_t timestamp, string_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::STRING) {
    }
    DataPoint(string_t key, uint64_t timestamp, bool_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::BOOLEAN) {
    }
    DataPoint(string_t key, uint64_t timestamp, uint8_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::UINT8) {
    }
    DataPoint(string_t key, uint64_t timestamp, uint16_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::UINT16) {
    }
    DataPoint(string_t key, uint64_t timestamp, uint32_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::UINT32) {
    }
    DataPoint(string_t key, uint64_t timestamp, uint64_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::UINT64) {
    }
    DataPoint(string_t key, uint64_t timestamp, int8_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::INT8) {
    }
    DataPoint(string_t key, uint64_t timestamp, int16_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::INT16) {
    }
    DataPoint(string_t key, uint64_t timestamp, int32_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::INT32) {
    }
    DataPoint(string_t key, uint64_t timestamp, int64_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::INT64) {
    }
    DataPoint(string_t key, uint64_t timestamp, float32_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::FLOAT32) {
    }
    DataPoint(string_t key, uint64_t timestamp, float64_t value)
        : key(std::move(key))
        , timestamp(timestamp)
        , value(value)
        , type(DataPointType::FLOAT64) {
    }

    inline string_t getString() {
        return boost::get<string_t>(value);
    }
    inline bool_t getBoolean() {
        return boost::get<bool_t>(value);
    }
    inline uint8_t getUInt8() {
        return boost::get<uint8_t>(value);
    }
    inline uint16_t getUInt16() {
        return boost::get<uint16_t>(value);
    }
    inline uint32_t getUInt32() {
        return boost::get<uint32_t>(value);
    }
    inline uint64_t getUInt64() {
        return boost::get<uint64_t>(value);
    }
    inline int8_t getInt8() {
        return boost::get<int8_t>(value);
    }
    inline int16_t getInt16() {
        return boost::get<int16_t>(value);
    }
    inline int32_t getInt32() {
        return boost::get<int32_t>(value);
    }
    inline int64_t getInt64() {
        return boost::get<int64_t>(value);
    }
    inline float32_t getFloat32() {
        return boost::get<float32_t>(value);
    }
    inline float64_t getFloat64() {
        return boost::get<float64_t>(value);
    }

    std::string toJson() {
        json val = {{"timestamp", timestamp}, {"key", key}, {"value", toString()}};
        return val.dump() + "\n";
    }
    std::string toDebugJson() {
        json val = {{"timestamp", timestamp}, {"key", key}, {"value", toString()}, {"type", (int)type}};
        return val.dump() + "\n";
    }

    bool isStringType() {
        switch (type) {
            case DataPointType::EVENT:
            case DataPointType::STRING:
            case DataPointType::BOOLEAN: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    bool isIntegerType() {
        switch (type) {
            case DataPointType::UINT8:
            case DataPointType::UINT16:
            case DataPointType::UINT32:
            case DataPointType::UINT64:
            case DataPointType::INT8:
            case DataPointType::INT16:
            case DataPointType::INT32:
            case DataPointType::INT64: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    bool isDecimalType() {
        switch (type) {
            case DataPointType::FLOAT32:
            case DataPointType::FLOAT64: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    std::string toString() {
        switch (type) {
            case DataPointType::EVENT: {
                return "true";
            }
            case DataPointType::STRING: {
                return getString();
            }
            case DataPointType::BOOLEAN: {
                return getBoolean() ? "true" : "false";
            }
            case DataPointType::UINT8: {
                return fmt::format("{:d}", getUInt8());
            }
            case DataPointType::UINT16: {
                return fmt::format("{:d}", getUInt16());
            }
            case DataPointType::UINT32: {
                return fmt::format("{:d}", getUInt32());
            }
            case DataPointType::UINT64: {
                return fmt::format("{:d}", getUInt64());
            }
            case DataPointType::INT8: {
                return fmt::format("{:d}", getInt8());
            }
            case DataPointType::INT16: {
                return fmt::format("{:d}", getInt16());
            }
            case DataPointType::INT32: {
                return fmt::format("{:d}", getInt32());
            }
            case DataPointType::INT64: {
                return fmt::format("{:d}", getInt64());
            }
            case DataPointType::FLOAT32: {
                return fmt::format("{:f}", getFloat32());
            }
            case DataPointType::FLOAT64: {
                return fmt::format("{:f}", getFloat64());
            }
            default: {
                return "";
            }
        }
    }
};

std::shared_ptr<DataPoint>
createDataPointFromString(const string_t& key, uint64_t timestamp, const string_t& stringValue);

#endif