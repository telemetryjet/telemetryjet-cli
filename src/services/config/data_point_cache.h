#ifndef TELEMETRYSERVER_DATA_POINT_CACHE_H
#define TELEMETRYSERVER_DATA_POINT_CACHE_H

#include "data_point_cache.h"
#include "core/data_point.h"
#include <unordered_map>

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

#endif //TELEMETRYSERVER_DATA_POINT_CACHE_H
