/**
 * Main Program Entry Point
 * Starts and runs the server loop.
 */
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

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
        std::cout << "TelemetryJet CLI, Version 0.0.1";
        return 0;
    }

    if (updateCommand->parsed()) {
        std::cout << "No updates available.";
        return 0;
    }

    return 0;
}