//
// Created by Chris Dalke on 1/29/20.
//

#ifndef TELEMETRYSERVER_DATACACHE_H
#define TELEMETRYSERVER_DATACACHE_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

struct data_key_t {
    long lastUpdated;
    long expireAge;
    std::string key;
    float value;
    float max_value;
    float min_value;
    float avg_value;
    long avg_value_quantity;
    bool expired;
};

class DataCache {
private:
    std::unordered_map<std::string, data_key_t> cache;
    std::vector<std::string> keys;
public:
    DataCache();
    float getFloat(const std::string& key);
    float setFloat(const std::string& key, float value);
    //bool getValueExpired(const std::string& key);

    std::vector<std::string> getKeys();
};

#endif //TELEMETRYSERVER_DATACACHE_H
