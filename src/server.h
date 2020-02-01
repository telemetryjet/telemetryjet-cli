#ifndef TELEMETRYSERVER_SERVER_H
#define TELEMETRYSERVER_SERVER_H

#include <device/deviceManager.hpp>
#include <data/dataCache.h>
#include <data/database.h>
#include "simpleTimer.hpp"
#include <iostream>

class Server {
private:
    DataCache dataCache;
    Database database;
    DeviceManager deviceManager;
    SimpleTimer timer;
    bool running = true;
public:
    Server();
    ~Server();
    void start();
    void update();
    bool shouldUpdate();
    void stop();
};
#endif
