#include "key_value_stream.h"
#include <string>
#include <boost/algorithm/string.hpp>

void KeyValueStream::update() {
    SerialStreamDataSource::update();

    if (!serial->getBuffer().empty()) {
        for (auto& inChar : serial->getBuffer()) {
            if (inChar == '\n') {
                try {
                    std::pair<std::string, std::string> keyPair = parseKeyValueBuffer(keyValueBuffer);
                    uint64_t timestamp = getCurrentMillis();
                    write(createDataPointFromString(keyPair.first, timestamp, keyPair.second));
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

    // send incoming data points to serial stream
    if (online && !in.empty()) {
        for (auto& dp : in) {
            serial->writeLine(fmt::format("{}={}", dp->key, dp->getString()));
        }
    }
}
