//
// Created by Chris Dalke on 1/29/20.
//

#ifndef TELEMETRYSERVER_DATACACHE_H
#define TELEMETRYSERVER_DATACACHE_H

#include <string>
#include <unordered_map>
#include <iostream>

class DataCache {
private:
    std::unordered_map<std::string, float> cache;
public:
    DataCache();
    float getFloat(const std::string& key);
    float setFloat(const std::string& key, float value);
};

#endif //TELEMETRYSERVER_DATACACHE_H
