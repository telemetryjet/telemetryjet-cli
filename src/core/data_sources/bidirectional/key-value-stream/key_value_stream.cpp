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
                    std::pair<std::string, std::string> keyPair = parseKeyValueBuffer(keyValueBuffer);
                    float valueFloat = std::stof(keyPair.second);
                    uint64_t timestamp = getCurrentMillis();
                    out.push_back(std::make_shared<DataPoint>(fmt::format("{}.{}",id,keyPair.first), valueFloat, timestamp));
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
