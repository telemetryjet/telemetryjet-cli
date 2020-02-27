#ifndef TELEMETRYSERVER_CONFIG_H
#define TELEMETRYSERVER_CONFIG_H
#include <string>

/**
 * Config
 * Interface for a configuration wrapper.
 * Provides key/values for options configured by the end user.
 * Should be instantiated before any other services.
 */
class Config {
public:
    virtual ~Config() = default;
    virtual bool hasInt(std::string key) = 0;
    virtual bool hasString(std::string key) = 0;
    virtual bool hasBool(std::string key) = 0;
    virtual int getInt(std::string key, int defaultVal) = 0;
    virtual std::string getString(std::string key, std::string defaultVal) = 0;
    virtual bool getBool(std::string key, bool defaultVal) = 0;
};

#endif