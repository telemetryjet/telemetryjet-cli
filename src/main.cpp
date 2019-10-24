#include "deviceManager.hpp"
#include "simpleTimer.hpp"
#include <iostream>

DeviceManager deviceManager;
SimpleTimer   timer(1000);

void shutdown();

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    shutdown();
    exit(signum);
}

int main() {
    std::cout << "Starting Telemetry Server...\n";

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    while (true) {
        if (timer.check()) {
            deviceManager.pollDevices();
        }
    }

    return 0;
}

void shutdown() {
    std::cout << "Stopping Telemetry Server...\n";
    deviceManager.shutdown();
}