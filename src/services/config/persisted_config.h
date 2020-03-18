#ifndef TELEMETRYSERVER_PERSISTED_CONFIG_H
#define TELEMETRYSERVER_PERSISTED_CONFIG_H

#include "config.h"
#include <unordered_map>

/**
 * Persisted Config
 * Implementation of the config service that saves/loads data from the database.
 * Allows config to be persisted between restarts.
 */
class PersistedConfig : public Config {
private:
    std::unordered_map<std::string, std::string> cache;
    void refreshCache();
public:
    bool hasInt(std::string key) override;
    bool hasString(std::string key) override;
    bool hasBool(std::string key) override;
    int getInt(std::string key, int defaultVal) override;
    std::string getString(std::string key, std::string defaultVal) override;
    bool getBool(std::string key, bool defaultVal) override;
    void setInt(std::string key, int val) override;
    void setString(std::string key, std::string val) override;
    void setBool(std::string key, bool val) override;
};

#endif //TELEMETRYSERVER_PERSISTED_CONFIG_H
