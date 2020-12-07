#include "data_cache.h"

void DataPointCache::set(const std::string& key, const std::shared_ptr<DataPoint>& dp) {
    map[key] = dp;
}

std::shared_ptr<DataPoint> DataPointCache::get(const std::string& key) {
    return map[key];
}

void DataPointCache::clear() {
    map.clear();
}

DataPointCache::DataPointCache() = default;
DataPointCache::~DataPointCache() = default;
