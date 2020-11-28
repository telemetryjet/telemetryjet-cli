/**
 * Main Program Entry Point
 * Starts and runs the server loop.
 */
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "src/version.h"

int main(int argc, char** argv) {
    CLI::App app{"TelemetryJet CLI"};

    auto versionCommand = app.add_subcommand("version", "Display the version and exit.")->group("Metadata");
    auto updateCommand = app.add_subcommand("update", "Check for updates and install if available.")->group("Metadata");
    auto serverCommand = app.add_subcommand("server", "Manage TelemetryJet server connections.")->group("Outputs");
    auto serverListCommand = serverCommand->add_subcommand("list", "List the available server connections.")->group("Outputs");
    auto serverClearCommand = serverCommand->add_subcommand("clear", "Clear all the server connections.")->group("Outputs");
    auto serverAddCommand = serverCommand->add_subcommand("add", "Add a server connection.")->group("Outputs");
    auto serverRemoveCommand = serverCommand->add_subcommand("remove", "Remove a server connection.")->group("Outputs");

    auto streamCommand = app.add_subcommand("stream", "Connect to one or more devices.")->group("Streaming");

    CLI11_PARSE(app, argc, argv);

    if (versionCommand->parsed()) {
        std::cout << "TelemetryJet CLI (version " << CLI_VERSION_MAJOR << "." << CLI_VERSION_MINOR << "." << CLI_VERSION_PATCH << ", platform " << CLI_VERSION_SYSTEM << ", architecture " << CLI_VERSION_ARCH << ")\n";
        return 0;
    }

    if (updateCommand->parsed()) {
        std::cout << "No updates available.";
        return 0;
    }

    std::cout << "TelemetryJet CLI (version " << CLI_VERSION_MAJOR << "." << CLI_VERSION_MINOR << "." << CLI_VERSION_PATCH << ", platform " << CLI_VERSION_SYSTEM << ", architecture " << CLI_VERSION_ARCH << ")\n";
    std::cout << "No command was specified, exiting.\n";

    return 0;
}