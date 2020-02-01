//
// Created by Chris Dalke on 2/1/20.
//

#ifndef TELEMETRYSERVER_APISERVER_H
#define TELEMETRYSERVER_APISERVER_H


#include <data/dataCache.h>
#include <data/database.h>
#include <device/deviceManager.hpp>

#include "server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

class ApiServer {
private:
    DataCache& cache;
    Database& database;
    DeviceManager& deviceManager;
    HttpServer server;
    std::thread serverThread;
    void _runServerThread();
public:
    ApiServer(DataCache& cache, Database &database, DeviceManager &deviceManager);
    ~ApiServer();
};

#endif //TELEMETRYSERVER_APISERVER_H
