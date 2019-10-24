#include "deviceManager.hpp"
#include "simpleTimer.hpp"
#include <iostream>

int main() {
    std::cout << "Starting Telemetry Server...\n";

    DeviceManager deviceManager;
    SimpleTimer   timer(1000);

    while (true) {
        if (timer.check()) {
            deviceManager.pollDevices();
        }
    }

    return 0;
}
