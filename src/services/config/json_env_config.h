#ifndef TELEMETRYSERVER_JSON_ENV_CONFIG_H
#define TELEMETRYSERVER_JSON_ENV_CONFIG_H

#include <unordered_map>
#include "config.h"

/**
 * JsonEnvConfig
 * Pulls from JSON configuration files, and then from environment variables
 * Pulls from the following hierarchy of configuration sources (later = higher)
 * ~/.telemetryjet (User telemetryjet config)
 * ./.telemetryjet (Project telemetryjet config)
 * Environment Variables (Overrides JSON values)
 *
 * Only supports flat JSON!
 */
class JsonEnvConfig : public Config {
private:
    std::unordered_map<std::string, std::string> stringValues;
    void loadConfig(const std::string& userConfig, const std::string &localConfig);
public:
    JsonEnvConfig();
    ~JsonEnvConfig() override;
    bool hasInt(std::string key) override;
    bool hasString(std::string key) override;
    virtual bool hasBool(std::string key) override;
    int getInt(std::string key, int defaultVal) override;
    std::string getString(std::string key, std::string defaultVal) override;
    bool getBool(std::string key, bool defaultVal) override;
    virtual void setInt(std::string key, int val) override;
    virtual void setString(std::string key, std::string val) override;
    virtual void setBool(std::string key, bool val) override;
};


#endif //TELEMETRYSERVER_JSON_ENV_CONFIG_H
