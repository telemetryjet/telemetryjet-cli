#include "json_env_config.h"
#include <fmt/format.h>
#include <unistd.h>
#include <services/service_manager.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;

#define TELEMETRYJET_CONFIG_FILENAME ".telemetryjet"

bool JsonEnvConfig::hasInt(std::string key) {
    return intValues.count(key) > 0;
}

bool JsonEnvConfig::hasString(std::string key) {
    return stringValues.count(key) > 0;
}

int JsonEnvConfig::getInt(std::string key, int defaultVal) {
    if (hasInt(key)) {
        return intValues[key];
    } else {
        return defaultVal;
    }
}

std::string JsonEnvConfig::getString(std::string key, std::string defaultVal) {
    if (hasString(key)) {
        return stringValues[key];
    } else {
        return defaultVal;
    }
}

// Load the config file hierarchy into the map
void JsonEnvConfig::loadConfig(const std::string& userConfig, const std::string &localConfig) {
    ServiceManager::getLogger()->info("Loading config file hierarchy...");
    ServiceManager::getLogger()->info(fmt::format("Global (User) Config: {}",userConfig));
    ServiceManager::getLogger()->info(fmt::format("Local Config: {}",localConfig));

    std::vector<std::string> keysFromUserConfig;
    std::vector<std::string> keysFromLocalConfig;

    pt::ptree propertyTreeUser;
    pt::ptree propertyTreeLocal;

    // Read configuration for user
    bool hasUserConfig = false;
    try {
        pt::read_json(userConfig, propertyTreeUser);
        hasUserConfig = true;
    } catch (pt::ptree_error &e) {
        ServiceManager::getLogger()->error(fmt::format("Failed to load user config ({})", e.what()));
    }

    // Read configuration for local
    bool hasLocalConfig = false;
    try {
        pt::read_json(localConfig, propertyTreeLocal);
        hasLocalConfig = true;
    } catch (pt::ptree_error &e) {
        ServiceManager::getLogger()->error(fmt::format("Failed to load local config ({})", e.what()));
    }

    if (!hasUserConfig && !hasLocalConfig) {
        ServiceManager::getLogger()->warning("No configuration files were loaded.");
    }
}


JsonEnvConfig::JsonEnvConfig() {
    char pathBuffer[1024];
    getcwd(pathBuffer,1024);

    std::string userConfigFilename = fmt::format("~/{}",TELEMETRYJET_CONFIG_FILENAME);
    std::string localConfigFilename = fmt::format("{}/{}",pathBuffer,TELEMETRYJET_CONFIG_FILENAME);

    loadConfig(userConfigFilename, localConfigFilename);
}

JsonEnvConfig::~JsonEnvConfig() = default;
