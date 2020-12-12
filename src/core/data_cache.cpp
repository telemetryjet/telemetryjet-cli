#include "data_cache.h"

void DataPointCache::set(const std::string& key, const std::shared_ptr<DataPoint>& dp) {
    map[key] = dp;
    if (keys.count(key) == 0) {
        keys.insert(key);
    }
}

std::shared_ptr<DataPoint> DataPointCache::get(const std::string& key) {
    return map[key];
}

std::unordered_set<std::string> DataPointCache::getKeys() {
    return keys;
}

void DataPointCache::clear() {
    map.clear();
    keys.clear();
}

DataPointCache::DataPointCache() = default;
DataPointCache::~DataPointCache() = default;
