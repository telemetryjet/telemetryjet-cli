#include "key_value_file_output.h"

KeyValueFileOutputDataSource::KeyValueFileOutputDataSource(const std::string& id, const json &options)
    : FileOutputDataSource(id, "key-value-file-output", options) {
    equalsEscapeRegex = std::basic_regex("=");
}

void KeyValueFileOutputDataSource::update() {
    if (!isOpen) {
        return;
    }
    if (outputFile.is_open()) {
        for (auto& inDataPoint : in) {
            std::string keyEscaped = std::regex_replace(inDataPoint->key, equalsEscapeRegex, "\\=");
            std::string valueEscaped = std::regex_replace(fmt::format("{}",inDataPoint->value), equalsEscapeRegex, "\\=");
            std::string keyValue = fmt::format("{}={}\n", keyEscaped,valueEscaped);
            outputFile << keyValue;
        }
    }
    if (flushTimer) {
        outputFile.flush();
    }
}