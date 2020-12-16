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
                float valueFloat = std::stof(keyPair.second);
                uint64_t timestamp = getCurrentMillis();
                out.push_back(std::make_shared<DataPoint>(keyPair.first, valueFloat, timestamp));
            } catch (std::exception &e) {
            }
            keyValueBuffer.clear();
        }
        if (!fileOpen) {
            isDoneReading = true;
        }
    }
}
