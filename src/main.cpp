#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "services/service_manager.h"
#include "src/version.h"
#include <algorithm>
#include <fmt/format.h>

void printServerList(const std::vector<record_server_t>& servers) {
    int maxAliasLength = 5;  // length of "ALIAS"
    int maxHostLength = 4;   // length of "HOST"
    for (const record_server_t& server : servers) {
        maxAliasLength = std::max(maxAliasLength, static_cast<int>(server.alias.length()));
        maxHostLength = std::max(maxHostLength, static_cast<int>(server.host.length()));
    }

    std::cout << fmt::format("ALIAS{}    HOST{}    USERNAME\n",
                             std::string(maxAliasLength - 5, ' '),
                             std::string(maxHostLength - 4, ' '));

    for (record_server_t server : servers) {
        std::cout << fmt::format("{}{}    {}{}    {}\n",
                                 server.alias,
                                 std::string(maxAliasLength - server.alias.length(), ' '),
                                 server.host,
                                 std::string(maxHostLength - server.host.length(), ' '),
                                 server.username);
    }
}

void printVersion() {
    std::cout << fmt::format("TelemetryJet CLI (version {}.{}.{}, platform {}, architecture {})\n",
                             CLI_VERSION_MAJOR,
                             CLI_VERSION_MINOR,
                             CLI_VERSION_PATCH,
                             CLI_VERSION_SYSTEM,
                             CLI_VERSION_ARCH);
}

int main(int argc, char** argv) {
    CLI::App app{"TelemetryJet CLI"};

    std::string serverAlias;
    std::string serverHost;
    std::string serverUsername;
    std::string serverPassword;
    bool confirmation;

    auto versionCommand
        = app.add_subcommand("version", "Display the version and exit.")->group("Metadata");
    auto updateCommand = app.add_subcommand("update", "Check for updates and install if available.")
                             ->group("Metadata");
    auto serverCommand
        = app.add_subcommand("server", "Manage TelemetryJet server connections.")->group("Outputs");

    // server subcommands
    auto serverAddCommand
        = serverCommand->add_subcommand("add", "Add a server connection.")->group("Outputs");
    serverAddCommand->add_option("--alias", serverAlias, "Specify an alias for the server.")
        ->required();
    serverAddCommand->add_option("--host", serverHost, "Specify the url of the server instance.")
        ->required();
    serverAddCommand->add_option("--username", serverUsername, "Specify the username.")->required();
    serverAddCommand->add_option("--password", serverPassword, "Specify the password.")->required();

    auto serverListCommand
        = serverCommand->add_subcommand("list", "List the available server connections.")
              ->group("Outputs");
    auto serverRemoveCommand
        = serverCommand->add_option("--remove", serverAlias, "Remove a server connection.")
              ->group("Outputs");

    auto serverClearCommand
        = serverCommand->add_subcommand("clear", "Clear all the server connections.")
              ->group("Outputs");
    serverClearCommand->add_flag("-y,--yes", confirmation, "Skip confirmation prompts.");

    auto streamCommand
        = app.add_subcommand("stream", "Connect to one or more devices.")->group("Streaming");

    CLI11_PARSE(app, argc, argv);

    if (versionCommand->parsed()) {
        printVersion();
        return 0;
    }

    if (updateCommand->parsed()) {
        std::cout << "No updates available.\n";
        return 0;
    }

    // server command implementations

    if (*serverListCommand) {
        SM::init();

        std::vector<record_server_t> servers = SM::getDatabase()->getAllServers();
        printServerList(servers);

        return 0;
    }

    if (*serverClearCommand) {
        SM::init();

        char input = 0;
        while (input != 'Y' && input != 'n' && !confirmation) {
            std::cout << "Are you sure you want to remove all servers? (Y/n) ";
            std::cin >> input;
        }

        if (input == 'Y' || confirmation) {
            SM::getDatabase()->deleteAllServers();
            std::cout << "All servers removed from list.\n";
        }

        return 0;
    }

    if (*serverAddCommand) {
        SM::init();

        if (SM::getDatabase()->serverExists(serverAlias)) {
            std::cout << "The alias " << serverAlias << " is already in use.\n";
            return 0;
        }

        record_server_t::createServer(serverAlias, serverHost, serverUsername, serverPassword);
        std::cout << "Server " << serverAlias << " successfully created.\n";
        return 0;
    }

    if (*serverRemoveCommand) {
        SM::init();

        if (SM::getDatabase()->serverExists(serverAlias)) {
            SM::getDatabase()->deleteServer(serverAlias);
            std::cout << "Server " << serverAlias << " successfully removed.\n";
        } else {
            std::cout << "The alias " << serverAlias << " does not exist.\n";
        }

        return 0;
    }

    printVersion();
    std::cout << "No command was specified, exiting.\n";

    return 0;
}