#include "key_value_stream.h"
#include <string>
#include <boost/algorithm/string.hpp>

KeyValueStream::KeyValueStream(const std::string& id, const json &options) : DataSource(id, "key-value-stream") {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("key-value-stream data source '{}' requires an options object.", id));
    }
    if (!options.contains("port") || !options["port"].is_string()) {
        throw std::runtime_error(fmt::format("key-value-stream data source '{}' requires option 'port' of type String.", id));
    }
    if (!options.contains("baudrate") || !options["baudrate"].is_number_integer()) {
        throw std::runtime_error(fmt::format("key-value-file data source '{}' requires option 'baudrate' of type Integer.", id));
    }
    portName = options["port"];
    baudRate = options["baudrate"];
}

KeyValueStream::~KeyValueStream() {

}

void KeyValueStream::close() {
    if (serial->isOpen()) {
        serial->close();
    }

    delete reconnectTimer;
}

void KeyValueStream::open() {
    serial = std::make_unique<SerialWrapper>(portName, baudRate);
    reconnectTimer = new SimpleTimer(5000);
}

void KeyValueStream::update() {
    if (serial->isOpen()) {
        // If serial is open, read data from buffer
        serial->poll();
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

    if (reconnectTimer->check() && !serial->isOpen()) {
        // Periodically try to open if it is not already open
        serial->open();
    }
}

bool KeyValueStream::isOpen() {
    return false;
}
