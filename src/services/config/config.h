#ifndef TELEMETRYSERVER_JSON_ENV_CONFIG_H
#define TELEMETRYSERVER_JSON_ENV_CONFIG_H

#include <string>
#include <unordered_map>
#include "config.h"

/**
 * Config
 * Provides key/values for options configured by the end user.
 * Should be instantiated before any other services.
 */
class Config {
private:
    std::unordered_map<std::string, std::string> stringValues;
public:
    Config();
    ~Config();
    bool hasInt(const std::string& key);
    bool hasString(const std::string& key);
    bool hasBool(const std::string& key);
    int getInt(const std::string& key, int defaultVal);
    std::string getString(const std::string& key, std::string defaultVal);
    bool getBool(const std::string& key, bool defaultVal);
    void setInt(const std::string& key, int val);
    void setString(const std::string& key, const std::string& val);
    void setBool(const std::string& key, bool val);
};

#endif //TELEMETRYSERVER_JSON_ENV_CONFIG_H
