#ifndef TELEMETRYJETCLI_WEBSOCKET_SERVER_H
#define TELEMETRYJETCLI_WEBSOCKET_SERVER_H

#include "core/data_source.h"
#include "server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class WebsocketServerDataSource : public DataSource {
private:
    WsServer server;
    std::thread serverThread;
    int port;
    std::string endpoint;
public:
    WebsocketServerDataSource(const json &definition);
    ~WebsocketServerDataSource() = default;
    void open() override;
    void close() override;
    void update() override;
};

#endif
