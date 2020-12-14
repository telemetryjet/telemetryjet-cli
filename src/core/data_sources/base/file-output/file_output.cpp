#include "file_output.h"
#include "utility/path_utils.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

FileOutputDataSource::FileOutputDataSource(const std::string& id, const std::string& type, const json &options)
    : DataSource(id, type) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires an options object.", type, id));
    }
    if (!options.contains("filename") || !options["filename"].is_string()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires option 'filename' of type String.", type, id));
    }
    if (!options.contains("mode") || !options["mode"].is_string()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires option 'mode' of type String.", type, id));
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
        throw std::runtime_error(fmt::format("{} data source '{}' has invalid value for 'mode': {}.", type, id, modeString));
    }
}

FileOutputDataSource::~FileOutputDataSource() {
}

void FileOutputDataSource::open() {
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
        SM::getLogger()->error(fmt::format("Failed to open {} file {}.", type, pathObj.filename().string()));
        throw std::runtime_error(fmt::format("Failed to open {} file {}.", type, pathObj.filename().string()));
    } else {
        SM::getLogger()->info(fmt::format("Opened {} file {}.", type, pathObj.filename().string()));
    }
    flushTimer = new SimpleTimer(1000);
}

void FileOutputDataSource::close() {
    if (outputFile.is_open()){
        outputFile.close();
    }
    delete flushTimer;
}

bool FileOutputDataSource::isOpen() {
    return outputFile.is_open();
}
