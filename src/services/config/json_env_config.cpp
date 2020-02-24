#include "json_env_config.h"
#include <fmt/format.h>
#include <unistd.h>
#include <regex>
#include <services/service_manager.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <string>
namespace pt = boost::property_tree;

#define TELEMETRYJET_CONFIG_FILENAME ".telemetryjet"
#define TELEMETRYJET_CONFIG_ENV_PREFIX "TELEMETRYJET_"

extern char **environ;

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

    std::unordered_map<std::string, std::string> userConfigKeypairs;
    if (hasUserConfig) {
        BOOST_FOREACH(pt::ptree::value_type &v, propertyTreeUser) {
                        std::string key = v.first;
                        boost::to_lower(key);
                        userConfigKeypairs[key] = v.second.data();
                    }
    }
    ServiceManager::getLogger()->debug(fmt::format("Found {} TelemetryJet configuration parameter(s) in user configuration file.",userConfigKeypairs.size()));

    std::unordered_map<std::string, std::string> localConfigKeypairs;
    if (hasLocalConfig) {
        BOOST_FOREACH(pt::ptree::value_type &v, propertyTreeLocal) {
            std::string key = v.first;
            boost::to_lower(key);
            localConfigKeypairs[key] = v.second.data();
        }
    }
    ServiceManager::getLogger()->debug(fmt::format("Found {} TelemetryJet configuration parameter(s) in local configuration file.",localConfigKeypairs.size()));

    // Read configuration from all environment variables with telemetryjet prefix
    // Currently using posix-only API, will need to add Windows solution
    std::unordered_map<std::string, std::string> rawEnvKeypairs;
    std::string prefix = std::string(TELEMETRYJET_CONFIG_ENV_PREFIX);
    std::locale loc;
    if (environ != nullptr) {
        char *current = *environ;
        for (int i = 1; current; i++){
            // Check list of all environment variables and save any with telemetryjet prefix
            std::string currentPair = std::string(current);
            std::string key = currentPair.substr(0, currentPair.find('='));
            std::string value = currentPair.substr(currentPair.find('=') + 1);
            if (key.rfind(prefix,0) == 0){
                std::string keyNoPrefix = key.substr(prefix.size());
                boost::to_lower(keyNoPrefix);
                rawEnvKeypairs[keyNoPrefix] = value;
            }
            current = *(environ+i);
        }
        ServiceManager::getLogger()->debug(fmt::format("Found {} TelemetryJet configuration parameter(s) in environment.",rawEnvKeypairs.size()));
        for (const auto& pair : rawEnvKeypairs) {
            auto key = pair.first;
            auto val = pair.second;
        }
    } else {
        ServiceManager::getLogger()->error("Environment variable configuration unavailable!");
        ServiceManager::getLogger()->error("No environment configuration was loaded.");
    }

    // Merge keypairs
    for (const auto& pair : userConfigKeypairs) {
        stringValues[pair.first] = pair.second;
    }
    for (const auto& pair : localConfigKeypairs) {
        stringValues[pair.first] = pair.second;
    }
    for (const auto& pair : rawEnvKeypairs) {
        stringValues[pair.first] = pair.second;
    }

    // Debug: Output the merged key pairs
    ServiceManager::getLogger()->info(fmt::format("{} configuration parameter(s) after merging.",stringValues.size()));
    for (const auto& pair : stringValues) {
        ServiceManager::getLogger()->debug(fmt::format(" - {}: {}",pair.first,pair.second));
    }
}


JsonEnvConfig::JsonEnvConfig() {
    ServiceManager::getLogger()->info("--------- Initializing Configuration ---------");
    char pathBuffer[1024];
    getcwd(pathBuffer,1024);

    std::string userConfigFilename = fmt::format("~/{}",TELEMETRYJET_CONFIG_FILENAME);
    std::string localConfigFilename = fmt::format("{}/{}",pathBuffer,TELEMETRYJET_CONFIG_FILENAME);

    loadConfig(userConfigFilename, localConfigFilename);
    ServiceManager::getLogger()->info("---------- Initialized Configuration ---------");
}

JsonEnvConfig::~JsonEnvConfig() = default;
