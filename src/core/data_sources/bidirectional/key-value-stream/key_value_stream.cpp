#include "key_value_stream.h"
#include <string>
#include <boost/algorithm/string.hpp>

KeyValueStream::KeyValueStream(const std::string& id, const json &options)
    : SerialStreamDataSource(id, "key-value-stream", options) {
}

void KeyValueStream::update() {
    if (!isOpen) {
        return;
    }
    SerialStreamDataSource::update();

    if (!serial->getBuffer().empty()) {
        std::string tempLine;
        for (auto& inChar : serial->getBuffer()) {
            if (inChar == '\n') {
                try {
                    // Process the line and clear input buffer
                    std::string key;
                    std::string value;
                    bool hasEscapeCharacter = false;
                    bool hasCompleteKey = false;
                    std::string lineVal;
                    for (auto& chr : keyValueBuffer) {
                        lineVal += (char)chr;
                    }
                    //SM::getLogger()->info(fmt::format("Input buffer: [{}]", lineVal));

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
                    float valueFloat = std::stof(value);
                    uint64_t timestamp = getCurrentMillis();
                    out.push_back(std::make_shared<DataPoint>(fmt::format("{}.{}",id,key), valueFloat, timestamp));
                } catch (std::exception &e) {
                }
                keyValueBuffer.clear();
            } else {
                if (inChar != '\r') {
                    keyValueBuffer.push_back(inChar);
                }
            }
        }
        serial->clearBuffer();
    }
}
