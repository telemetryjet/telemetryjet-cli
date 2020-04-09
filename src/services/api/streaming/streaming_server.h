#ifndef TELEMETRYSERVER_STREAMING_SERVER_H
#define TELEMETRYSERVER_STREAMING_SERVER_H
#include "server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class StreamingServer {
    WsServer *server;
    std::thread serverThread;
    void _runServerThread();
public:
    StreamingServer();
    ~StreamingServer();
    void sendMessageToAll(std::string message);
};

#endif //TELEMETRYSERVER_STREAMING_SERVER_H
