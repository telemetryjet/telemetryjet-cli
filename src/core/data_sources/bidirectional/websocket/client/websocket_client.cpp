#include "websocket_client.h"

WebsocketClientDataSource::WebsocketClientDataSource(const json& definition)
    : DataSource(definition)
    , path(getServerPath())
    , client(path) {
    online = false;
}

void WebsocketClientDataSource::open() {
    client.on_open = [this](const std::shared_ptr<WsClient::Connection>& connection) {
        wsConnection = connection;
        online = true;
        SM::getLogger()->info(
            fmt::format("[{}] Opened websocket client connection to server at {}.", id, path));
    };

    client.on_close = [this](const std::shared_ptr<WsClient::Connection>& connection,
                             int status,
                             const std::string& reason) {
        SM::getLogger()->info(
            fmt::format("[{}] Closed websocket client connection to server at {} with status code {}.",
                        id,
                        path,
                        status));
        online = false;
        clientThreadWantsExit = true;
    };

    client.on_error = [this](const std::shared_ptr<WsClient::Connection>& connection,
                             const SimpleWeb::error_code& ec) {
        SM::getLogger()->error(
            fmt::format("[{}] Error in websocket client connection to server at {}. {}",
                        id,
                        path,
                        ec.message()));
        clientThreadWantsExit = true;
    };

    client.on_message = [this](const std::shared_ptr<WsClient::Connection>& connection,
                               const std::shared_ptr<WsClient::InMessage>& message) {
        try {
            auto jsonObj = json::parse(message->string());
            if (!jsonObj.contains("key") || !jsonObj.contains("timestamp")
                || !jsonObj.contains("value")) {
                throw std::runtime_error(
                        fmt::format("Unable to parse message from websocket server: {}",
                                    message->string()));
            }
            write(createDataPointFromString(jsonObj["key"], jsonObj["timestamp"], jsonObj["value"]));
        } catch (std::exception& e) {
            SM::getLogger()->warning(
                fmt::format("[{}] Warning: Failed to decode WebSocket client message", id));

        }
    };

    startClientThread();
    reconnectTimer = std::make_unique<SimpleTimer>(5000);
    DataSource::open();
}

void WebsocketClientDataSource::startClientThread() {
    if (!clientThreadRunning) {
        wsConnection.reset();
        online = false;
        clientThreadRunning = true;
        clientThreadWantsExit = false;
        clientThread = boost::thread([this]() { client.start(); });
    }
}

void WebsocketClientDataSource::stopClientThread() {
    if (clientThreadRunning) {
        online = false;
        if (wsConnection != nullptr) {
            wsConnection->send_close(1000);
            wsConnection.reset();
        }
        client.stop();
        clientThread.join();
        clientThreadRunning = false;
        clientThreadWantsExit = false;
    }
}

void WebsocketClientDataSource::close() {
    stopClientThread();
    DataSource::close();
}

void WebsocketClientDataSource::update() {
    if (clientThreadWantsExit) {
        stopClientThread();
        return;
    }

    // send incoming data points to ws server
    if (online && !in.empty()) {
        for (auto& dp : in) {
            wsConnection->send(dp->toJson());
        }
    }
    if (!online && reconnectTimer->check()) {
        startClientThread();
    }
}

void WebsocketClientDataSource::checkOnline() {
    // TODO: set online member variable
}

std::string WebsocketClientDataSource::getServerPath() {
    if (!options.contains("server_port_path")) {
        throw std::runtime_error("Websocket client data source requires server_port_path option.");
    }

    return options["server_port_path"];
}
