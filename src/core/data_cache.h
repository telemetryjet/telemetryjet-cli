#ifndef DATA_CACHE_H
#define DATA_CACHE_H

#include "core/data_point.h"
#include <unordered_map>
#include <memory>

class DataPointCache {
private:
    std::unordered_map<std::string, std::shared_ptr<DataPoint>> map;
public:
    DataPointCache();
    ~DataPointCache();

    void set(const std::string& key, const std::shared_ptr<DataPoint>& dp);
    std::shared_ptr<DataPoint> get(const std::string& key);
    void clear();
};

#endif