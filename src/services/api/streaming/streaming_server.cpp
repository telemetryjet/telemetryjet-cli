#include <services/service_manager.h>
#include <fmt/format.h>
#include "streaming_server.h"
#include "fmt/ostream.h"

// Convert a shared pointer to a string address for debugging purposes
inline std::string pointerToString(const std::shared_ptr<WsServer::Connection>& connection){
    WsServer::Connection *connectionPtr = connection.get();
    char pstr[sizeof(connectionPtr) * 2 + 2 + 1]; // Each byte of the pointer is two hexadecimal character, plus a potential 0x, plus terminator
    snprintf(pstr, sizeof(pstr), "%p", (void *) connectionPtr);
    return fmt::format("{}",pstr);
}

StreamingServer::StreamingServer() {
    SM::getLogger()->info("Starting Streaming Server...");

    server = new WsServer();
    server->config.port = SM::getConfig()->getInt("streaming_port",9001);

    // Configure streaming API endpoint
    auto &endpoint = server->endpoint["^/api$"];


    endpoint.on_open = [](const std::shared_ptr<WsServer::Connection>& connection) {
        SM::getLogger()->info(fmt::format("Streaming server: Opened connection {}",pointerToString(connection)));
    };

    endpoint.on_close = [](const std::shared_ptr<WsServer::Connection>& connection, int status, const std::string &reason) {
        SM::getLogger()->info(fmt::format("Streaming server: Closed connection {} with status code {}",pointerToString(connection), status));
    };

    endpoint.on_handshake = [](const std::shared_ptr<WsServer::Connection>& connection, SimpleWeb::CaseInsensitiveMultimap & response_header) {
        return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
    };

    endpoint.on_error = [](const std::shared_ptr<WsServer::Connection>& connection, const SimpleWeb::error_code &ec) {
        SM::getLogger()->info(fmt::format("Streaming server: Error in connection {}. Error: {}, Message: {}",pointerToString(connection), ec, ec.message()));
    };

    endpoint.on_message = [](std::shared_ptr<WsServer::Connection> connection, std::shared_ptr<WsServer::InMessage> in_message) {
        SM::getStreamingServer()->sendMessageToAll(in_message->string());
    };

    serverThread = std::thread{&StreamingServer::_runServerThread, this};
}

StreamingServer::~StreamingServer() {
    SM::getLogger()->info("Stopping Streaming Server...");

    // Close connections first
    for(auto &connection : server->get_connections()) {
        connection->send_close(1000);
    }

    server->stop();
    serverThread.join();
    delete server;
    SM::getLogger()->info("Stopped Streaming Server.");
}

void StreamingServer::_runServerThread() {
    server->start([](unsigned short port) {
        SM::getLogger()->info(fmt::format("Started Streaming Server on port {}.", port));
    });
}

void StreamingServer::sendMessageToAll(std::string message) {
    for(auto &connection : server->get_connections()) {
        connection->send(message);
    }
}