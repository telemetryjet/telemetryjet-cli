#include "server.h"

Server::Server() :
    dataCache(DataCache()),
    database(Database(dataCache)),
    deviceManager(DeviceManager()),
    timer(SimpleTimer(1000)) {
}

Server::~Server()= default;

void Server::start() {
    std::cout << "Started Telemetry Server! \n";
}


void Server::update() {

    if (timer.check()) {
        deviceManager.update();
    }
    deviceManager.getData();
}

bool Server::shouldUpdate() {
    return running;
}

void Server::stop() {
    deviceManager.shutdown();
    std::cout << "Stopped Telemetry Server! \n";
}