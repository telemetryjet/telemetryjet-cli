#ifndef TELEMETRYSERVER_CACHE_H
#define TELEMETRYSERVER_CACHE_H

#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <fmt/format.h>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * DataCache
 * Interface for a data cache
 * Stores raw data values
 * Used for all getting/setting raw data points
 */
class DataCache {
public:
    DataCache();
    ~DataCache();

    enum Key : int {
        // system usage
        SYS_DISK_AVAIL = 0,
        SYS_DISK_USED = 1,
        SYS_MEM_AVAIL = 2,
        SYS_MEM_USED = 3,
        SYS_CPU_USED = 4,
        SYS_CPU_TEMP = 5,
        SYS_GPU_USED = 6,
        SYS_GPU_TEMP = 7,
        // nmea 0183
        GPS_FIX_STATUS = 8,
        GPS_FIX_TYPE = 9,
        GPS_FIX_QUALITY = 10,
        GPS_FIX_ALTITUDE = 11,
        GPS_FIX_LATITUDE = 12,
        GPS_FIX_LONGITUDE = 13,
        GPS_FIX_SPEED = 14,
        GPS_FIX_TRAVELANGLE = 15,
        GPS_FIX_TRACKINGSATELLITES = 16,
        GPS_FIX_VISIBLESATELLITES = 17,
        GPS_FIX_TIMESTAMP_RAWTIME = 18
    };

    template <class T>
    void set(const int key, T value) {
        if (cache.find(key) == cache.end()) {
            keys.push_back(key);
        }
        cache[key] = value;
    }

    template <class T>
    T get(const int key) {
        if (cache.find(key) == cache.end()) {
            return 0;
        } else {
            return boost::get<T>(cache[key]);
        }
    }

    std::vector<int> getKeys() {
        return keys;
    }

    boost::property_tree::ptree toPropertyTree() {
        boost::property_tree::ptree pt;
        for (const int key : keys) {
            auto var = cache[key];
            if (var.type() == typeid(float)) {
                pt.add(std::to_string(key), get<float>(key));
            } else if (var.type() == typeid(uint64_t)) {
                pt.add(std::to_string(key), get<uint64_t>(key));
            } else {
                throw std::runtime_error(
                    "Cache variant type not implemented in DataCache::toPropertyTree()");
            }
        }
        return pt;
    }

private:
    std::unordered_map<int, boost::variant<float, uint64_t>> cache;
    std::vector<int> keys;
};

#endif  // TELEMETRYSERVER_CACHE_H
