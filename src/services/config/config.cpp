#include "config.h"
#include <fmt/format.h>
#include <services/service_manager.h>
#include <string>

// Int value is backed by string value
// Parsed when we go to get the config value
// The performance should be enhanced here through memoizing the int value
bool Config::hasInt(const std::string& key) {
    // Check if we have a string value
    if (hasString(key)){
        // Parse the value to see if it can be used as an int
        try {
            std::stoi(stringValues[key]);
            return true;
        } catch (std::exception &e) {
            return false;
        }
    } else {
        return false;
    }
}

int Config::getInt(const std::string& key, int defaultVal) {
    if (hasString(key)) {
        try {
            return std::stoi(stringValues[key]);
        } catch (std::exception &e) {
            return defaultVal;
        }
    } else {
        return defaultVal;
    }
}

bool Config::hasString(const std::string& key) {
    return stringValues.count(key) > 0;
}

bool Config::hasBool(const std::string& key) {
    if (stringValues.count(key) > 0) {
        std::string val = stringValues[key];
        return val == "true" || val == "false";
    } else {
        return false;
    }
}

std::string Config::getString(const std::string& key, std::string defaultVal) {
    if (hasString(key)) {
        return stringValues[key];
    } else {
        return defaultVal;
    }
}

bool Config::getBool(const std::string& key, bool defaultVal) {
    if (hasBool(key)) {
        return stringValues[key] == "true";
    } else {
        return defaultVal;
    }
}

void Config::setInt(const std::string& key, int val) {
    setString(key, fmt::format("{}",val));
}

void Config::setString(const std::string& key, const std::string& val) {
    ServiceManager::getLogger()->info(fmt::format("Set config keypair: [{}: {}]", key, val));
    stringValues[key] = val;
}

void Config::setBool(const std::string& key, bool val) {
    setString(key, val ? "true" : "false");
}

Config::Config() = default;
Config::~Config() = default;
