#ifndef TELEMETRYSERVER_SERVER_H
#define TELEMETRYSERVER_SERVER_H

#include <device/deviceManager.hpp>
#include <data/dataCache.h>
#include <data/database.h>
#include "simpleTimer.hpp"
#include <iostream>
#include <server/apiServer.h>

class Server {
private:
    void stop();
public:
    DataCache cache;
    Database database;
    DeviceManager deviceManager;
    ApiServer apiServer;
    bool running = true;
    Server();
    void start();
    void triggerStop();
};
#endif
