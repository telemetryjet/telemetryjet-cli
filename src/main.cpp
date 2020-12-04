#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "services/service_manager.h"
#include "src/version.h"
#include <fmt/format.h>
#include <filesystem>
#include <boost/filesystem.hpp>
#include "utility/glob/glob.h"
#include "utility/path_utils.h"

void printVersion() {
    SM::getLogger()->info(fmt::format("TelemetryJet CLI (version {}.{}.{}, platform {}, architecture {})",
                             CLI_VERSION_MAJOR,
                             CLI_VERSION_MINOR,
                             CLI_VERSION_PATCH,
                             CLI_VERSION_SYSTEM,
                             CLI_VERSION_ARCH));
}

int main(int argc, char** argv) {
    CLI::App app{"TelemetryJet CLI"};
    app.set_config();
    SM::init();

    bool versionFlag;
    bool silentFlag;
    app.add_flag("-v,--version", versionFlag, "Display the version and exit");
    app.add_flag("-s,--silent", silentFlag, "Don't log any debug or error messages");

    auto streamCommand = app.add_subcommand("stream", "Create and run a network of data sources.")->group("Streaming");
    std::vector<std::string> configurationFileGlobs;
    streamCommand->add_option("configurations", configurationFileGlobs, "Configuration files to load.");

    CLI11_PARSE(app, argc, argv);

    if (silentFlag) {
        SM::getLogger()->setLevel(LoggerLevel::LEVEL_NONE);
    }

    // If the version flag was specified, display the version and exit.
    if (versionFlag) {
        printVersion();
        SM::destroy();
        return 0;
    }

    if (!streamCommand->parsed()) {
        // Display the version and quit if no other commands were entered.
        printVersion();
        SM::getLogger()->info("No command was specified, exiting...");
        SM::destroy();
        return 0;
    }

    std::string currentPath = boost::filesystem::current_path().generic_string();

    // Load configuration files
    std::vector<std::filesystem::path> configurationFilePaths;
    for (auto& configFileGlob : configurationFileGlobs) {
        std::string absolutePathGlob = resolveRelativePathHome(configFileGlob);
        SM::getLogger()->info(fmt::format("Rel path: [{}] -> abs path: [{}]", configFileGlob, absolutePathGlob));
        for (std::filesystem::path& path : glob::rglob(absolutePathGlob)) {
            std::filesystem::file_status fileStatus = std::filesystem::status(path);
            if (std::filesystem::is_regular_file(fileStatus) || std::filesystem::is_symlink(fileStatus)) {
                configurationFilePaths.push_back(path);
            } else {
                SM::getLogger()->warning(fmt::format("{} is not a regular file, ignoring...",path.string()));
            }
        }
    }

    if (configurationFilePaths.empty()) {
        // Display the version and quit if no configuration files were entered.
        SM::getLogger()->info("No configuration files were specified, exiting...");
        SM::destroy();
        return 0;
    }

    for (auto& configFilename : configurationFilePaths) {
        SM::getLogger()->info(configFilename.generic_string());
    }
}