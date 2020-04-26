#include "streaming_server.h"
#include "utility/json_utils.h"
#include <constants.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <services/service_manager.h>

using namespace boost::property_tree;

// Convert a shared pointer to a string address for debugging purposes
inline std::string pointerToString(const std::shared_ptr<WsServer::Connection>& connection) {
    WsServer::Connection* connectionPtr = connection.get();
    // Each byte of the pointer is two hexadecimal character, plus a potential 0x, plus terminator
    char pstr[sizeof(connectionPtr) * 2 + 2 + 1];
    snprintf(pstr, sizeof(pstr), "%p", (void*)connectionPtr);
    return fmt::format("{}", pstr);
}

StreamingServer::StreamingServer() {
    SM::getLogger()->info("Starting Streaming Server...");

    server = new WsServer();
    server->config.port = SM::getConfig()->getInt("streaming_port", 9001);

    // Configure streaming API endpoint
    auto& endpoint = server->endpoint["^/api$"];

    endpoint.on_open = [](const std::shared_ptr<WsServer::Connection>& connection) {
        SM::getLogger()->info(
            fmt::format("Streaming server: Opened connection {}", pointerToString(connection)));
    };

    endpoint.on_close = [](const std::shared_ptr<WsServer::Connection>& connection,
                           int status,
                           const std::string& reason) {
        SM::getLogger()->info(
            fmt::format("Streaming server: Closed connection {} with status code {}",
                        pointerToString(connection),
                        status));
    };

    endpoint.on_handshake = [](const std::shared_ptr<WsServer::Connection>& connection,
                               SimpleWeb::CaseInsensitiveMultimap& response_header) {
        return SimpleWeb::StatusCode::information_switching_protocols;  // Upgrade to websocket
    };

    endpoint.on_error = [](const std::shared_ptr<WsServer::Connection>& connection,
                           const SimpleWeb::error_code& ec) {
        SM::getLogger()->info(
            fmt::format("Streaming server: Error in connection {}. Error: {}, Message: {}",
                        pointerToString(connection),
                        ec,
                        ec.message()));
    };

    endpoint.on_message = [](const std::shared_ptr<WsServer::Connection>& connection,
                             const std::shared_ptr<WsServer::InMessage>& in_message) {
        try {
            ptree pt = stringToPropertyTree(in_message->string());
            boost::optional<ptree&> messageType = pt.get_child_optional(INCOMING_MESSAGE_TYPE_KEY);
            if (!messageType) {
                throw std::runtime_error(
                    fmt::
                        format("Streaming server: Received a message that is missing the {} field.",
                               INCOMING_MESSAGE_TYPE_KEY));
            }

            int type = pt.get<int>(INCOMING_MESSAGE_TYPE_KEY);

            boost::optional<ptree&> messageBody = pt.get_child_optional(INCOMING_MESSAGE_BODY_KEY);
            if (!messageBody) {
                throw std::runtime_error(
                    fmt::
                        format("Streaming server: Received a message that is missing the {} field.",
                               INCOMING_MESSAGE_BODY_KEY));
            }

            handleIncomingMessage(type, messageBody.get(), connection);

        } catch (std::exception& error) {
            SM::getLogger()->error(error.what());
        }
    };

    serverThread = std::thread{&StreamingServer::_runServerThread, this};
}

StreamingServer::~StreamingServer() {
    SM::getLogger()->info("Stopping Streaming Server...");

    // Close connections first
    for (auto& connection : server->get_connections()) {
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

void StreamingServer::handleIncomingMessage(
    int type,
    const boost::property_tree::ptree& body,
    const std::shared_ptr<WsServer::Connection>& connection) {

    switch (static_cast<IncomingMessageType>(type)) {
        case IncomingMessageType::TEST_MESSAGE:
            SM::getLogger()->info(fmt::format("Received message with type {}", type));
            record_log_t::createLog("test", LOG_LEVEL_DEBUG);
            break;
        default:
            SM::getLogger()->error(
                fmt::format("Streaming server: Received message with unrecognized {} {}.",
                            INCOMING_MESSAGE_TYPE_KEY,
                            type));
    }
}

void StreamingServer::sendMessageToAll(OutgoingMessageType type,
                                       const boost::property_tree::ptree& body) {
    boost::property_tree::ptree root;
    root.add(OUTGOING_MESSAGE_TYPE_KEY, static_cast<int>(type));
    root.put_child(OUTGOING_MESSAGE_BODY_KEY, body);

    for (auto& connection : server->get_connections()) {
        connection->send(propertyTreeToString(root));
    }
}
