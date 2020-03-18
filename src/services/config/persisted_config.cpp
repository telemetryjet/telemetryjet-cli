#include "persisted_config.h"
#include <services/service_manager.h>
#include <fmt/format.h>

// Load all values from the database table into cache
// Prevents unnecessary calls to the database
void PersistedConfig::refreshCache() {
    cache.clear();
    //SM::getLogger()->info("Refreshed config cache");
    for (const auto &configItem :  SM::getDatabase()->getConfigItems()) {
        cache[configItem.key] = configItem.value;
        //SM::getLogger()->info(fmt::format("{} = {}",configItem.key, configItem.value));
    }
}

bool PersistedConfig::hasInt(std::string key) {
    refreshCache();
    // Check if we have a string value
    if (hasString(key)){
        // Parse the value to see if it can be used as an int
        try {
            std::stoi(cache[key]);
            return true;
        } catch (std::exception &e) {
            return false;
        }
    } else {
        return false;
    }
}

int PersistedConfig::getInt(std::string key, int defaultVal) {
    refreshCache();
    if (hasString(key)) {
        try {
            return std::stoi(cache[key]);
        } catch (std::exception &e) {
            return defaultVal;
        }
    } else {
        return defaultVal;
    }
}

bool PersistedConfig::hasString(std::string key) {
    refreshCache();
    return cache.count(key) > 0;
}

std::string PersistedConfig::getString(std::string key, std::string defaultVal) {
    refreshCache();
    if (hasString(key)) {
        return cache[key];
    } else {
        return defaultVal;
    }
}

bool PersistedConfig::hasBool(std::string key) {
    refreshCache();
    if (cache.count(key) > 0) {
        std::string val = cache[key];
        if (val == "true" || val == "false"){
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool PersistedConfig::getBool(std::string key, bool defaultVal) {
    refreshCache();
    if (hasBool(key)) {
        return cache[key] == "true";
    } else {
        return defaultVal;
    }
}

void PersistedConfig::setInt(std::string key, int val) {
    setString(key, fmt::format("{}",val));

}

void PersistedConfig::setBool(std::string key, bool val) {
    setString(key, val ? "true" : "false");
}

void PersistedConfig::setString(std::string key, std::string val) {
    if (hasString(key)) {
        SM::getDatabase()->updateConfigItem({
            key,
            val
        });
    } else {
        SM::getDatabase()->createConfigItem({
            key,
            val
        });
    }
    refreshCache();
}