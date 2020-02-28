#ifndef TELEMETRYSERVER_REST_API_SERVER_IMPL_H
#define TELEMETRYSERVER_REST_API_SERVER_IMPL_H

#include <thread>
#include "rest_api_server.h"
#include "server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

class RestApiServerImpl : public RestApiServer {
private:
    HttpServer *server;
    std::thread serverThread;
    void _runServerThread();
public:
    RestApiServerImpl();
    ~RestApiServerImpl() override;
};

#endif //TELEMETRYSERVER_REST_API_SERVER_IMPL_H
