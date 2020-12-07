#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "services/service_manager.h"
#include "src/version.h"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "utility/glob/glob.h"
#include "utility/path_utils.h"
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include "core/network.h"
#include <libserialport.h>

using json = nlohmann::json;

void printVersion() {
    SM::getLogger()->info(fmt::format("TelemetryJet CLI (version {}.{}.{}, platform {}, architecture {})",
                             CLI_VERSION_MAJOR,
                             CLI_VERSION_MINOR,
                             CLI_VERSION_PATCH,
                             CLI_VERSION_SYSTEM,
                             CLI_VERSION_ARCH));
}

void printSerialPorts() {
        struct sp_port **port_list;
        enum sp_return result = sp_list_ports(&port_list);

        if (result != SP_OK) {
            SM::getLogger()->error("Could not list serial ports!");
            return;
        }

        for (int i = 0; port_list[i] != NULL; i++) {
            struct sp_port *port = port_list[i];
            char *port_name = sp_get_port_name(port);
            char *port_desc = sp_get_port_description(port);
            enum sp_transport transport = sp_get_port_transport(port);
            std::string transportString = "Unknown";
            SM::getLogger()->info(fmt::format(" - {}, {}, {}", port_name, port_desc));
            if (transport == SP_TRANSPORT_NATIVE) {
                SM::getLogger()->info(fmt::format("   - Transport: Software"));
            } else if (transport == SP_TRANSPORT_USB) {
                int usb_vid, usb_pid;
                sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);
                int usb_bus, usb_address;
                sp_get_port_usb_bus_address(port, &usb_bus, &usb_address);

                SM::getLogger()->info(fmt::format("   - Transport: USB"));
                SM::getLogger()->info(fmt::format("   - Manufacturer: {}", sp_get_port_usb_manufacturer(port)));
                SM::getLogger()->info(fmt::format("   - Product: {}", sp_get_port_usb_product(port)));
                SM::getLogger()->info(fmt::format("   - Serial: {}", sp_get_port_usb_serial(port)));
                SM::getLogger()->info(fmt::format("   - VID: {}", usb_vid));
                SM::getLogger()->info(fmt::format("   - PID: {}", usb_pid));
                SM::getLogger()->info(fmt::format("   - Bus: {}", usb_bus));
                SM::getLogger()->info(fmt::format("   - Address: {}", usb_address));
            } else if (transport == SP_TRANSPORT_BLUETOOTH) {
                SM::getLogger()->info(fmt::format("   - Transport: Bluetooth"));
                SM::getLogger()->info(fmt::format("   - MAC Address: {}", sp_get_port_bluetooth_address(port)));
            }
        }
        sp_free_port_list(port_list);
}

int main(int argc, char** argv) {
    CLI::App app{"TelemetryJet CLI"};
    app.set_config();
    SM::init();

    bool versionFlag;
    bool silentFlag;
    bool dryRunFlag;
    app.add_flag("-v,--version", versionFlag, "Display the version and exit");
    app.add_flag("-s,--silent", silentFlag, "Don't log any debug or error messages");

    auto streamCommand = app.add_subcommand("stream", "Create and run a network of data sources.")->group("Streaming");
    std::vector<std::string> configurationFileGlobs;
    streamCommand->add_option("configurations", configurationFileGlobs, "Configuration files to load.");
    streamCommand->add_flag("-s,--silent", silentFlag, "Don't log any debug or error messages");
    streamCommand->add_flag("-t,--test", dryRunFlag, "Test configuration and exit without running");

    auto listSerialPortsCommand = app.add_subcommand("list-ports", "List available serial ports.")->group("Serial Ports");

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

    if (listSerialPortsCommand->parsed()) {
        printSerialPorts();
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

    if (configurationFileGlobs.empty()) {
        SM::getLogger()->info("No configuration files were specified, exiting...");
        SM::destroy();
        return 0;
    }

    // Load configuration files
    std::vector<boost::filesystem::path> configurationFilePaths;
    int numIgnoredMatches = 0;
    for (auto& configFileGlob : configurationFileGlobs) {
        std::string absolutePathGlob = resolveRelativePathHome(configFileGlob);
        //SM::getLogger()->info(fmt::format("Rel path: [{}] -> abs path: [{}]", configFileGlob, absolutePathGlob));
        for (boost::filesystem::path& path : glob::rglob(absolutePathGlob)) {
            boost::filesystem::file_status fileStatus = boost::filesystem::status(path);
            std::string extensionString = boost::algorithm::to_lower_copy(path.extension().string());
            if ((boost::filesystem::is_regular_file(fileStatus) || boost::filesystem::is_symlink(fileStatus)) && extensionString == ".json") {
                configurationFilePaths.push_back(path);
            } else {
                numIgnoredMatches++;
            }
        }
    }
    if (numIgnoredMatches > 0) {
        SM::getLogger()->warning(fmt::format("Matched {} non-JSON files, which were ignored.", numIgnoredMatches));
    }

    if (configurationFilePaths.empty()) {
        // Display the version and quit if no configuration files were entered.
        SM::getLogger()->info("No configuration files were found, exiting...");
        SM::destroy();
        return 0;
    }

    std::vector<json> jsonConfigFiles;
    bool errorFlag = false;
    for (auto& configFilename : configurationFilePaths) {
        boost::filesystem::path filePath(configFilename);
        try {
            boost::filesystem::ifstream inputFile(configFilename);
            json jsonConfigItem;
            inputFile >> jsonConfigItem;

            if (!jsonConfigItem.is_array()) {
                throw std::runtime_error("Configuration file must be a JSON array of data source definition objects. \nPlease see https://docs.dev.telemetryjet.com/cli/guides/usage/ for an overview of the configuration specification.");
            }
            for (auto& item : jsonConfigItem) {
                if (!item.is_object()) {
                    throw std::runtime_error("Configuration file must be a JSON array of data source definition objects. \nPlease see https://docs.dev.telemetryjet.com/cli/guides/usage/ for an overview of the configuration specification.");
                }
                if (!item.contains("id")) {
                    throw std::runtime_error("Data source requires 'id' field.");
                }
                if (!item.contains("type")) {
                    throw std::runtime_error(fmt::format("Error in data source '{}': 'type' field is required.", item["id"].get<std::string>()));
                }
                if (item.contains("type") && !item["type"].is_string()) {
                    throw std::runtime_error(fmt::format("Error in data source '{}': 'type' field must be of type String.", item["id"].get<std::string>()));
                }
                if (item.contains("prefix") && !item["prefix"].is_string()) {
                    throw std::runtime_error(fmt::format("Error in data source '{}': 'prefix' field must be of type String.", item["id"].get<std::string>()));
                }
            }

            jsonConfigFiles.push_back(jsonConfigItem);
        } catch (json::exception &e) {
            SM::getLogger()->error(fmt::format("Failed to parse JSON in configuration file '{}':", filePath.filename().string()));
            SM::getLogger()->error(e.what());
            errorFlag = true;
        } catch (std::exception &e) {
            SM::getLogger()->error(fmt::format("Failed to load configuration file '{}':", filePath.filename().string()));
            SM::getLogger()->error(e.what());
            errorFlag = true;
        }
    }

    if (errorFlag) {
        SM::destroy();
        return 0;
    }

    if (configurationFilePaths.size() == 1) {
        SM::getLogger()->info("Loaded 1 configuration file.");
    } else {
        SM::getLogger()->info(fmt::format("Loaded {} configuration files.", configurationFilePaths.size()));
    }

    std::vector<Network> networks;
    try {
        for (auto& jsonConfigFile : jsonConfigFiles) {
            Network newNetwork(jsonConfigFile);
            networks.push_back(newNetwork);
        }
    } catch (std::exception &e) {
        SM::getLogger()->error("Failed to initialize network:");
        SM::getLogger()->error(e.what());
        SM::destroy();
        return 0;
    }

    for (auto& network : networks) {
        network.start();
    }

    if (configurationFilePaths.size() == 1) {
        SM::getLogger()->info("Started streaming data for 1 configuration file (Ctrl-C to stop)");
    } else {
        SM::getLogger()->info(fmt::format("Started streaming data for {} configuration files (Ctrl-C to stop)", configurationFilePaths.size()));
    }
    bool shouldRun = true;
    while (shouldRun) {
        for (auto& network : networks) {
            network.update();
        }
        shouldRun = false;
        for (auto& network : networks) {
            if (network.isOpen()) {
                shouldRun = true;
            }
        }
    }

    for (auto& network : networks) {
        network.stop();
    }
}