#ifndef TELEMETRYSERVER_PATH_UTILS_H
#define TELEMETRYSERVER_PATH_UTILS_H

#include <cstdlib>
#include <string>
#include <fmt/format.h>
#include <boost/filesystem/path.hpp>

/**
 * resolveUserHome
 * Replaces a relative path to a user's home with the absolute path.
 * eg ~/test -> /Users/chris/test
 */
inline std::string resolveUserHome(std::string directory) {
    std::string homeDir = getenv("HOME");
    if (directory.length() == 0){
        return directory;
    }
    if (directory.length() == 1 && directory.front() == '~'){
        return homeDir;
    }
    if (directory.length() >= 2 && directory.substr(0,2) == "~/") {
        return fmt::format("{}{}",homeDir, directory.substr(1,directory.size() - 1));
    }
    return directory;
}

/**
 * Get folder
 * From a file path, strips the filename to get the lowest directory.
 * eg /usr/bin/run -> /usr/bin
 */
inline std::string getFolder(std::string path) {
    boost::filesystem::path tempPath(path);
    boost::filesystem::path dir = tempPath.parent_path();
    return dir.generic_string();
}

#endif //TELEMETRYSERVER_PATH_UTILS_H
