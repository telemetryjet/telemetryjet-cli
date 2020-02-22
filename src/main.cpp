#include <ctime>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <services/ServiceManager.h>
#include <utility/simpleTimer.hpp>
#include "services/ServiceManager.h"

namespace pt = boost::posix_time;

/**
 * Main Program Entry Point
 * Starts and runs the server loop.
 */

int exitCode = 0;
bool running = true;

/*
void threadTest(Server &server) {
    // Test writing data from a separate thread!
    SimpleTimer testTimer(100000);

    while (server.running){
        pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
        long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

        if (testTimer.check()){
            // Test: Write a sine wave to cache
            float sineWave = sin(milliseconds / 1000.0f) * 100.0f;
            std::cout << sineWave << "\n";
            ServiceManager::getCache().setFloat("sine", sineWave);
            ServiceManager::getCache().setFloat("sine2", -sineWave);
        }
    }
}
*/

void signalHandler(int signum) {
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize the common services.
    ServiceManager::init();

    SimpleTimer databaseTimer(1000);
    //std::thread testThread(threadTest, std::ref(*this));

    ServiceManager::getLogger().info("Started Telemetry Server!");

    // Run the server loop
    while (running) {
        pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
        long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

        if (databaseTimer.check()) {
            // At the update interval, grab all values from the data cache and save them into the database
            ServiceManager::getDatabase().startPacket();
            for (const std::string &key : ServiceManager::getCache().getKeys()) {
                std::string value = std::to_string(ServiceManager::getCache().getFloat(key));
                ServiceManager::getDatabase().createDataRow(milliseconds, key, value);
            }
            ServiceManager::getDatabase().endPacket();
        }
    }

    ServiceManager::getLogger().info("Stopped Telemetry Server.");

    // Shutdown the common services
    ServiceManager::destroy();

    // Exit main program
    return exitCode;
}