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
                    uint64_t timestamp = getCurrentMillis();
                    out.push_back(createDataPointFromString(fmt::format("{}.{}",id,keyPair.first), timestamp, keyPair.second));
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
