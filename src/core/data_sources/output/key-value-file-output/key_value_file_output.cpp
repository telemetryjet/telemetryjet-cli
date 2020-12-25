#include "key_value_file_output.h"

void KeyValueFileOutputDataSource::update() {
    for (auto& inDataPoint : in) {
        if (lastTimestamp < inDataPoint->timestamp) {
            lastTimestamp = inDataPoint->timestamp;
        }
    }
    if (!in.empty()) {
        std::string timestampRow = fmt::format("timestamp={}\n", lastTimestamp);
        outputFile << timestampRow;
        for (auto& inDataPoint : in) {
            std::string keyEscaped = std::regex_replace(inDataPoint->key, equalsEscapeRegex, "\\=");
            std::string valueEscaped = std::regex_replace(fmt::format("{}",inDataPoint->toString()), equalsEscapeRegex, "\\=");
            std::string keyValue = fmt::format("{}={}\n", keyEscaped,valueEscaped);
            outputFile << keyValue;
        }
    }
    if (flushTimer) {
        outputFile.flush();
    }
}