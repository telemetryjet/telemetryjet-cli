#include "key_value_file_input.h"

void KeyValueFileInputDataSource::update() {
    char inChar;
    bool endOfFile = false;
    if (inputFile.get(inChar)) {
        if (inChar != '\n' && inChar != '\r') {
            keyValueBuffer.push_back(inChar);
        }
    } else {
        endOfFile = true;
    }
    if (inChar == '\n' || endOfFile) {
        // Parse if we are at end of line or end of file
        try {
            std::pair<std::string, std::string> keyPair = parseKeyValueBuffer(keyValueBuffer);
            uint64_t timestamp = getCurrentMillis();
            write(createDataPointFromString(keyPair.first, timestamp, keyPair.second));
        } catch (std::exception &e) {
        }
        keyValueBuffer.clear();
    }
    if (endOfFile) {
        SM::getLogger()->info(fmt::format("[{}] Finished reading from {}.", id, filename));
        state = INACTIVE;
    }
}
