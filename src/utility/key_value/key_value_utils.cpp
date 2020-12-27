#include "key_value_utils.h"
#include <utility>
#include <boost/algorithm/string.hpp>

std::pair<std::string, std::string> parseKeyValueString(std::string keyValueString) {
    std::list<uint8_t> keyValueBuffer;
    for (auto& chr : keyValueString) {
        keyValueBuffer.push_back((uint8_t)chr);
    }
    return parseKeyValueBuffer(keyValueBuffer);
}

std::pair<std::string, std::string> parseKeyValueBuffer(std::list<uint8_t> keyValueBuffer) {
    std::string key;
    std::string value;
    bool hasCompleteKey = false;

    std::string lineVal;
    for (auto& chr : keyValueBuffer) {
        lineVal += (char)chr;
    }

    while (!keyValueBuffer.empty()) {
        if (!hasCompleteKey) {
            if (keyValueBuffer.front() == '='){
                hasCompleteKey = true;
                keyValueBuffer.pop_front();
            } else {
                key += (char)keyValueBuffer.front();
                keyValueBuffer.pop_front();
            }
        } else {
            value += (char)keyValueBuffer.front();
            keyValueBuffer.pop_front();
        }
    }

    // Trim whitespace from key and value
    boost::trim(key);
    boost::trim(value);
    if (key.empty() || value.empty()) {
        throw std::runtime_error("Couldn't find key or value in line!");
    }
    return std::make_pair(key, value);
}