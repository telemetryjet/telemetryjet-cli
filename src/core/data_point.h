#ifndef DATA_POINT_H
#define DATA_POINT_H

#include <utility>
#include <variant>
#include <string>
#include <iostream>
#include <boost/variant.hpp>
#include <fmt/format.h>

// Alias various types so we acn easily change their underlying type if needed
typedef std::string string_t;
typedef bool bool_t;
typedef float float32_t;
typedef double float64_t;
typedef boost::variant<
        string_t,
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
        float64_t> data_type_union_t;

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
    const string_t key;
    const uint64_t timestamp;
    const data_type_union_t value;
    const DataPointType type;

    DataPoint(string_t key, uint64_t timestamp):                  key(std::move(key)), timestamp(timestamp), type(DataPointType::EVENT) {}
    DataPoint(string_t key, uint64_t timestamp, string_t value):  key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::STRING)  {}
    DataPoint(string_t key, uint64_t timestamp, bool_t value):    key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::BOOLEAN) {}
    DataPoint(string_t key, uint64_t timestamp, uint8_t value):   key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::UINT8)   {}
    DataPoint(string_t key, uint64_t timestamp, uint16_t value):  key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::UINT16)  {}
    DataPoint(string_t key, uint64_t timestamp, uint32_t value):  key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::UINT32)  {}
    DataPoint(string_t key, uint64_t timestamp, uint64_t value):  key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::UINT64)  {}
    DataPoint(string_t key, uint64_t timestamp, int8_t value):    key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::INT8)    {}
    DataPoint(string_t key, uint64_t timestamp, int16_t value):   key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::INT16)   {}
    DataPoint(string_t key, uint64_t timestamp, int32_t value):   key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::INT32)   {}
    DataPoint(string_t key, uint64_t timestamp, int64_t value):   key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::INT64)   {}
    DataPoint(string_t key, uint64_t timestamp, float32_t value): key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::FLOAT32) {}
    DataPoint(string_t key, uint64_t timestamp, float64_t value): key(std::move(key)), timestamp(timestamp), value(value), type(DataPointType::FLOAT64) {}

    inline string_t  getString()  { return boost::get<string_t>(value);  }
    inline bool_t    getBoolean() { return boost::get<bool_t>(value); }
    inline uint8_t   getUInt8()   { return boost::get<uint8_t>(value);   }
    inline uint16_t  getUInt16()  { return boost::get<uint16_t>(value);  }
    inline uint32_t  getUInt32()  { return boost::get<uint32_t>(value);  }
    inline uint64_t  getUInt64()  { return boost::get<uint64_t>(value);  }
    inline int8_t    getInt8()    { return boost::get<int8_t>(value);    }
    inline int16_t   getInt16()   { return boost::get<int16_t>(value);   }
    inline int32_t   getInt32()   { return boost::get<int32_t>(value);   }
    inline int64_t   getInt64()   { return boost::get<int64_t>(value);   }
    inline float32_t getFloat32() { return boost::get<float32_t>(value); }
    inline float64_t getFloat64() { return boost::get<float64_t>(value); }

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
                return fmt::format("{}", getUInt8());
            }
            case DataPointType::UINT16: {
                return fmt::format("{}", getUInt16());
            }
            case DataPointType::UINT32: {
                return fmt::format("{}", getUInt32());
            }
            case DataPointType::UINT64: {
                return fmt::format("{}", getUInt64());
            }
            case DataPointType::INT8: {
                return fmt::format("{}", getInt8());
            }
            case DataPointType::INT16: {
                return fmt::format("{}", getInt16());
            }
            case DataPointType::INT32: {
                return fmt::format("{}", getInt32());
            }
            case DataPointType::INT64: {
                return fmt::format("{}", getInt64());
            }
            case DataPointType::FLOAT32: {
                return fmt::format("{}", getFloat32());
            }
            case DataPointType::FLOAT64: {
                return fmt::format("{}", getFloat64());
            }
            default: {
                return "";
            }
        }
    }
};

std::shared_ptr<DataPoint> createDataPointFromString(const string_t& key, uint32_t timestamp, const string_t& stringValue);

#endif