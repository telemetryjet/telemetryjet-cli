#include "file_input.h"
#include "utility/path_utils.h"

namespace fs = boost::filesystem;

FileInputDataSource::FileInputDataSource(const json &definition): DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires an options object.", type, id));
    }
    if (!options.contains("filename") || !options["filename"].is_string()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires option 'filename' of type String.", type, id));
    }
    filename = options["filename"];
}

void FileInputDataSource::open() {
    // Resolve filename to a path and open an input stream
    fs::path pathObj(resolveRelativePathHome(filename));
    inputFile = std::ifstream(pathObj.string(), std::ifstream::in);

    if (!inputFile.is_open()){
        SM::getLogger()->error(fmt::format("Failed to open {} file {}.", type, pathObj.filename().string()));
        throw std::runtime_error(fmt::format("Failed to open {} file {}.", type, pathObj.filename().string()));
    }

    SM::getLogger()->info(fmt::format("Opened {} file {}.", type, pathObj.filename().string()));
    DataSource::open();
}

void FileInputDataSource::close() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
    DataSource::close();
}