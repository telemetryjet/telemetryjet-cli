#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <cstdlib>
#include <string>
#include <fmt/format.h>
#include <boost/filesystem.hpp>

/**
 * resolveRelativePathHome
 * Replaces a relative path to a user's home with the absolute path.
 * Also converts relative paths (., ..) into absolute paths.
 * eg ~/test -> /Users/chris/test
 */
inline std::string resolveRelativePathHome(std::string directory) {
    #ifdef PLATFORM_WIN32
        char* userProfileEnv = getenv("USERPROFILE");
        char* userHomeDrive = getenv("HOMEDRIVE");
        char* userHomePath = getenv("HOMEPATH");
        std::string homeDir;
        if (userProfileEnv == nullptr) {
            homeDir = fmt::format("{}{}", userHomeDrive, userHomePath);
        } else {
            homeDir = std::string(userProfileEnv);
        }
    #else
        std::string homeDir(getenv("HOME"));
    #endif

    std::string newPath = directory;
    if (directory.length() == 0){
        newPath = directory;
    }
    if (directory.length() == 1 && directory.front() == '~'){
        newPath = homeDir;
    }
    if (directory.length() >= 2 && directory.substr(0,2) == "~/") {
        newPath = fmt::format("{}{}",homeDir, directory.substr(1,directory.size() - 1));
    }

    boost::filesystem::path canonicalPath = boost::filesystem::weakly_canonical(newPath);
    return canonicalPath.generic_string();
}

#endif