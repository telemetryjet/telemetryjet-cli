#include "key_value_file.h"

KeyValueFile::KeyValueFile(const std::string& id, const json &options)
    : OutputFileDataSource(id, "key-value-file", options) {
    equalsEscapeRegex = std::basic_regex("=");
}

void KeyValueFile::update() {
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

KeyValueFile::~KeyValueFile() = default;
