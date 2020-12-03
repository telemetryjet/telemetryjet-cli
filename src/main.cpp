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

bool handleBooleanPrompt(std::string promptMessage) {
    char input = 0;
    while (input != 'Y' && input != 'y' && input != 'n' && input != 'N') {
        std::cout << promptMessage << " (Y/n) ";
        std::cin >> input;
    }

    return (input == 'Y' || input == 'y');
}

std::string handleStringPrompt(std::string promptMessage, std::string errorMessage, std::string defaultValue) {
    std::string outputString;
    // If there is a default, only prompt once before using the default
    // If there is no default, force prompt until we get a value.
    while (outputString.empty()) {
        if (defaultValue.empty()) {
            std::cout << promptMessage << ": ";
        } else {
            std::cout << promptMessage << " [Default: " << defaultValue << "]: ";
        }
        getline(std::cin, outputString);
        if (outputString.empty() && !defaultValue.empty()) {
            return defaultValue;
        }
        if (outputString.empty()) {
            std::cout << errorMessage << "\n";
        }
    }
    return outputString;
}

int main(int argc, char** argv) {
    CLI::App app{"TelemetryJet CLI"};

    std::string serverAlias;
    std::string serverHost;
    std::string serverUsername;
    std::string serverPassword;
    bool skipConfirmationPromptsFlag;

    bool optionFlag;
    app.add_flag("-v,--version", optionFlag, "Display the version and exit");

    auto serverCommand = app.add_subcommand(
    "server",
"Manage TelemetryJet server connections."
                )->group("Outputs");

    // server subcommands
    auto serverAddCommand
        = serverCommand->add_subcommand("add", "Add a server connection.")->group("Outputs");
    serverAddCommand->add_option("--alias", serverAlias, "Specify an alias for the server.");
    serverAddCommand->add_option("--host", serverHost, "Specify the url of the server instance.");
    serverAddCommand->add_option("--username", serverUsername, "Specify the username.");
    serverAddCommand->add_option("--password", serverPassword, "Specify the password.");

    auto serverListCommand
        = serverCommand->add_subcommand("list", "List the available server connections.")
              ->group("Outputs");
    auto serverRemoveCommand
        = serverCommand->add_subcommand("remove", "Remove a server connection.")
              ->group("Outputs");
    serverRemoveCommand->add_option("--alias", serverAlias, "Specify a server alias to be removed.");
    serverRemoveCommand->add_flag("-y,--yes", skipConfirmationPromptsFlag, "Skip confirmation prompts.");

    auto serverClearCommand
        = serverCommand->add_subcommand("clear", "Clear all the server connections.")
              ->group("Outputs");
    serverClearCommand->add_flag("-y,--yes", skipConfirmationPromptsFlag, "Skip confirmation prompts.");

    auto streamCommand
        = app.add_subcommand("stream", "Connect to one or more devices.")->group("Streaming");

    CLI11_PARSE(app, argc, argv);

    if (optionFlag) {
        printVersion();
        return 0;
    }

    if (*serverListCommand) {
        SM::init();

        std::vector<record_server_t> servers = SM::getDatabase()->getAllServers();
        printServerList(servers);

        return 0;
    }

    if (*serverClearCommand) {
        SM::init();

        if (skipConfirmationPromptsFlag || handleBooleanPrompt("Are you sure you want to remove all servers?")) {
            SM::getDatabase()->deleteAllServers();
            std::cout << "All servers removed from list.\n";
        }

        return 0;
    }

    if (*serverAddCommand) {
        SM::init();

        if (serverAlias.empty()) {
            serverAlias = handleStringPrompt("Server alias", "A server alias is required.", "default");
        }
        if (SM::getDatabase()->serverExists(serverAlias)) {
            std::cout << "The server alias '" << serverAlias << "' is already in use.\n";
            return 0;
        }

        if (serverHost.empty()) {
            serverHost = handleStringPrompt("Server host", "A server host is required.", "app.telemetryjet.com");
        }
        if (serverUsername.empty()) {
            serverUsername = handleStringPrompt("Server username","A server username is required.", "");
        }
        if (serverPassword.empty()) {
            serverPassword = handleStringPrompt("Server password","A server password is required.", "");
        }

        record_server_t::createServer(serverAlias, serverHost, serverUsername, serverPassword);
        std::cout << "Server " << serverAlias << " successfully created.\n";
        return 0;
    }

    if (*serverRemoveCommand) {
        SM::init();

        if (serverAlias.empty()) {
            serverAlias = handleStringPrompt("Server alias", "A server alias is required.", "default");
        }

        if (SM::getDatabase()->serverExists(serverAlias)) {
            if (skipConfirmationPromptsFlag || handleBooleanPrompt(fmt::format("Are you sure you want to remove the server '{}'?", serverAlias))) {
                SM::getDatabase()->deleteServer(serverAlias);
                std::cout << "Server " << serverAlias << " successfully removed.\n";
            }
        } else {
            std::cout << "The alias " << serverAlias << " does not exist.\n";
        }

        return 0;
    }

    printVersion();
    std::cout << "No command was specified, exiting.\n";

    return 0;
}