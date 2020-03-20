#ifndef TELEMETRYSERVER_INMEMORYCACHE_H
#define TELEMETRYSERVER_INMEMORYCACHE_H

#include "cache.h"
#include <vector>
#include <unordered_map>

class InMemoryCache: public Cache {
private:
    std::unordered_map<std::string, float> cache;
    std::vector<std::string> keys;
public:
    InMemoryCache();
    ~InMemoryCache();
    float getFloat(const std::string& key) override;
    float setFloat(const std::string& key, float value) override;
    std::vector<std::string> getKeys() override;
};

#endif //TELEMETRYSERVER_INMEMORYCACHE_H
