#ifndef TELEMETRYSERVER_REST_API_SERVER_H
#define TELEMETRYSERVER_REST_API_SERVER_H

#include <thread>
#include "server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

class RestApiServer {
private:
    HttpServer *server;
    std::thread serverThread;
    void _runServerThread();
public:
    RestApiServer();
    ~RestApiServer();
};

#endif //TELEMETRYSERVER_REST_API_SERVER_H
