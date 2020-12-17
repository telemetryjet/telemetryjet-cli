#include "key_value_file_input.h"

KeyValueFileInputDataSource::KeyValueFileInputDataSource(const std::string& id,
                                                         const json& options)
    : FileInputDataSource(id, "key-value-file-input", options) {
}
void KeyValueFileInputDataSource::update() {
    if (isOpen) {
        char inChar;
        bool fileOpen = true;
        if (inputFile.get(inChar)) {
            if (inChar != '\n' && inChar != '\r') {
                keyValueBuffer.push_back(inChar);
            }
        } else {
            fileOpen = false;
        }
        if (inChar == '\n' || !fileOpen) {
            // Parse if we are at end of line or end of file
            try {
                std::pair<std::string, std::string> keyPair = parseKeyValueBuffer(keyValueBuffer);
                uint64_t timestamp = getCurrentMillis();
                out.push_back(createDataPointFromString(fmt::format("{}.{}",id,keyPair.first), timestamp, keyPair.second));
            } catch (std::exception &e) {
            }
            keyValueBuffer.clear();
        }
        if (!fileOpen) {
            isDoneReading = true;
        }
    }
}
