#include "file_input.h"

namespace fs = boost::filesystem;

FileInputDataSource::FileInputDataSource(const json &definition): DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires an options object", id, type));
    }
    if (!options.contains("filename") || !options["filename"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'filename' of type String", id, type));
    }
    filename = options["filename"];
    assertDependency("file", filename, fmt::format("[{}] Multiple data sources cannot share the same input/output filename: {}", id, filename));
    outputEnabled = false;
}

void FileInputDataSource::open() {
    // Resolve filename to a path and open an input stream
    fs::path pathObj(resolveRelativePathHome(filename));
    inputFile = std::ifstream(pathObj.string(), std::ifstream::in);

    if (!inputFile.is_open()){
        SM::getLogger()->error(fmt::format("[{}] Failed to open file {}", id, pathObj.filename().string()));
        throw std::runtime_error(fmt::format("[{}] Failed to open file {}", id, pathObj.filename().string()));
    }

    SM::getLogger()->info(fmt::format("[{}] Opened file {}", id, pathObj.filename().string()));
    DataSource::open();
}

void FileInputDataSource::close() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
    DataSource::close();
}