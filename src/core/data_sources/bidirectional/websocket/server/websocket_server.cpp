#include "websocket_server.h"

WebsocketServerDataSource::WebsocketServerDataSource(const json& definition)
    : DataSource(definition) {
    if (!options.contains("port")) {
        throw std::runtime_error(
            fmt::format("[{}] data source type '{}' requires option 'port' of type Integer",
                        id,
                        type));
    }

    if (!options.contains("endpoint")) {
        throw std::runtime_error(
            fmt::format("[{}] data source type '{}' requires option 'endpoint' of type String",
                        id,
                        type));
    }

    port = options["port"];
    endpoint = options["endpoint"];
}

void WebsocketServerDataSource::open() {
    server.config.port = port;
    auto& ws = server.endpoint[fmt::format("^/{}/?$", endpoint)];

    ws.on_open = [](const std::shared_ptr<WsServer::Connection>& connection) {
        SM::getLogger()->info("Opened websocket server connection.");
    };

    ws.on_close = [](const std::shared_ptr<WsServer::Connection>& connection,
                     int status,
                     const std::string& reason) {
        SM::getLogger()->info(
            fmt::format("Closed websocket server connection with status code {}.", status));
    };

    ws.on_error = [](const std::shared_ptr<WsServer::Connection>& connection,
                     const SimpleWeb::error_code& ec) {
        SM::getLogger()->error(
            fmt::format("Error in websocket server connection. {}", ec.message()));
    };

    ws.on_message = [this](const std::shared_ptr<WsServer::Connection>& connection,
                           const std::shared_ptr<WsServer::InMessage>& in_message) {
        auto jsonObj = json::parse(in_message->string());
        if (!jsonObj.contains("key") || !jsonObj.contains("timestamp")
            || !jsonObj.contains("value")) {
            throw std::runtime_error(
                fmt::format("Unable to parse message received by websocket server: {}",
                            in_message->string()));
        }
        write(createDataPointFromString(jsonObj["key"], jsonObj["timestamp"], jsonObj["value"]));
    };

    // Start server and receive assigned port when server is listening for requests
    std::promise<unsigned short> server_port;
    serverThread = boost::thread([this, &server_port]() {
        // Start server
        server.start([&server_port](unsigned short portVal) { server_port.set_value(portVal); });
    });

    SM::getLogger()->info(
        fmt::format("Websocket server listening on port {}", server_port.get_future().get()));
    DataSource::open();
}

void WebsocketServerDataSource::close() {
    // Close connections first
    for (auto& connection : server.get_connections()) {
        connection->send_close(1000);
    }

    server.stop();
    serverThread.join();
    DataSource::close();
}

void WebsocketServerDataSource::update() {
    // send incoming data points to all clients
    if (!in.empty()) {
        for (auto& dp : in) {
            for (auto& connection : server.get_connections()) {
                connection->send(dp->toJson());
            }
        }
    }
}