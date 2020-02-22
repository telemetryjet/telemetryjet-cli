//
// Created by Chris Dalke on 2/11/20.
//

#ifndef TELEMETRYSERVER_INMEMORYCACHE_H
#define TELEMETRYSERVER_INMEMORYCACHE_H

#include "Cache.h"
#include <unordered_map>
#include <data/dataTypes.h>

class InMemoryCache: public Cache {
private:
    std::unordered_map<std::string, data_key_t> cache;
    std::vector<std::string> keys;
public:
    InMemoryCache();
    float getFloat(const std::string& key) override;
    float setFloat(const std::string& key, float value) override;
    std::vector<std::string> getKeys() override;
};

#endif //TELEMETRYSERVER_INMEMORYCACHE_H
