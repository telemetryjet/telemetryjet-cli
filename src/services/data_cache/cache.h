#ifndef TELEMETRYSERVER_CACHE_H
#define TELEMETRYSERVER_CACHE_H

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
private:
    std::unordered_map<std::string, boost::variant<float, uint64_t>> cache;
    std::vector<std::string> keys;

public:
    DataCache();
    ~DataCache();

    template <class T>
    void set(const std::string& key, T value) {
        if (cache.find(key) == cache.end()) {
            keys.push_back(key);
        }
        cache[key] = value;
    }

    template <class T>
    T get(const std::string& key) {
        if (cache.find(key) == cache.end()) {
            return 0;
        } else {
            return boost::get<T>(cache[key]);
        }
    }

    std::vector<std::string> getKeys() {
        return keys;
    }
};

#endif  // TELEMETRYSERVER_CACHE_H
