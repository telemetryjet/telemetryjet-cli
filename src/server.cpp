#include "server.h"
#include <ctime>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <server/apiServer.h>

namespace pt = boost::posix_time;

Server::Server() :
        cache(DataCache()),
        database(Database()),
        deviceManager(DeviceManager()),
        apiServer(ApiServer(cache, database, deviceManager)) {
}

void threadTest(Server &server) {
    // Test writing data from a separate thread!
    SimpleTimer testTimer(1000);

    while (server.running){
        pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
        long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

        if (testTimer.check()){
            // Test: Write a sine wave to cache
            float sineWave = sin(milliseconds / 1000.0f) * 100.0f;
            std::cout << sineWave << "\n";
            server.cache.setFloat("sine", sineWave);
            server.cache.setFloat("sine2", -sineWave);
        }
    }
}

void Server::start() {
    std::cout << "Started Telemetry Server! \n";

    SimpleTimer deviceTimer(1000);
    SimpleTimer databaseTimer(100);

    std::thread testThread(threadTest, std::ref(*this));

    while (running) {
        pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
        long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

        if (deviceTimer.check()) {
            deviceManager.update();
        }
        deviceManager.getData();

        if (databaseTimer.check()){
            // At the update interval, grab all values from the data cache and save them into the database
            database.startPacket();
            for (const std::string& key : cache.getKeys()){
                std::string value = std::to_string(cache.getFloat(key));
                database.createDataRow(milliseconds, key, value);
            }
            database.endPacket();
        }
    }

    stop();
}

void Server::triggerStop() {
    running = false;
}

void Server::stop() {
    deviceManager.shutdown();
    std::cout << "Stopped Telemetry Server! \n";
}