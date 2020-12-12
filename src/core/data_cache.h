#ifndef DATA_CACHE_H
#define DATA_CACHE_H

#include "core/data_point.h"
#include <unordered_map>
#include <memory>
#include <unordered_set>

class DataPointCache {
private:
    std::unordered_map<std::string, std::shared_ptr<DataPoint>> map;
    std::unordered_set<std::string> keys;
public:
    DataPointCache();
    ~DataPointCache();

    void set(const std::string& key, const std::shared_ptr<DataPoint>& dp);
    std::shared_ptr<DataPoint> get(const std::string& key);
    std::unordered_set<std::string> getKeys();
    void clear();
};

#endif