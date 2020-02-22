#ifndef TELEMETRYSERVER_CONFIG_H
#define TELEMETRYSERVER_CONFIG_H
#include <string>

/**
 * Config
 * Interface for a configuration wrapper.
 * Provides key/values for options used across the project.
 * Should be instantiated before any other services.
 */
class Config {
public:
    virtual int getInt(std::string key);
};

#endif //TELEMETRYSERVER_CONFIG_H
