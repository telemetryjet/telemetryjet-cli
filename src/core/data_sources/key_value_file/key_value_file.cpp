#include "key_value_file.h"
#include "utility/path_utils.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

KeyValueFile::KeyValueFile(std::string id, const json &options) : DataSource(id, "key-value-file") {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("key-value-file data source '{}' requires an options object.", id));
    }
    if (!options.contains("filename") || !options["filename"].is_string()) {
        throw std::runtime_error(fmt::format("key-value-file data source '{}' requires option 'filename' of type String.", id));
    }
    if (!options.contains("mode") || !options["mode"].is_string()) {
        throw std::runtime_error(fmt::format("key-value-file data source '{}' requires option 'mode' of type String.", id));
    }
    filename = options["filename"];
    modeString = options["mode"];
    if (modeString == "overwrite") {
        mode = std::ios::trunc;
    } else if (modeString == "append") {
        mode = std::ios::app;
    } else if (modeString == "new") {
        mode = std::ios::app;
        generateNewFile = true;
    } else {
        throw std::runtime_error(fmt::format("key-value-file data source '{}' has invalid value for 'mode': {}.", id, modeString));
    }

    equalsEscapeRegex = std::basic_regex("=");
}

KeyValueFile::~KeyValueFile() {
}

void KeyValueFile::open() {
    // If we are in new file mode, increment filename until the file does not exist
    std::string absPath = resolveRelativePathHome(filename);
    fs::path pathObj(absPath);
    std::string fileName = pathObj.stem().string();
    std::string fileExt = pathObj.extension().string();
    int newFileId = 0;
    if (modeString == "new") {
        do {
            std::string newFilename = fmt::format("{}_{}{}",fileName, newFileId++, fileExt);
            pathObj.remove_filename();
            pathObj.append(newFilename);
        } while (fs::exists(pathObj));
    }
    filename = pathObj.string();
    outputFile = std::ofstream();
    outputFile.open(filename, mode);
    if (!outputFile.is_open()){
        SM::getLogger()->error(fmt::format("Failed to open Key-Value file {}.", pathObj.filename().string()));
        throw std::runtime_error(fmt::format("Failed to open Key-Value file {}.", pathObj.filename().string()));
    } else {
        SM::getLogger()->info(fmt::format("Opened Key-Value file {}.", pathObj.filename().string()));
    }
    flushTimer = new SimpleTimer(1000);
}

void KeyValueFile::close() {
    if (outputFile.is_open()){
        outputFile.close();
    }
    delete flushTimer;
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

bool KeyValueFile::isOpen() {
    return !!outputFile;
}
