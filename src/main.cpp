#include "logger.hpp"
#include "simpleTimer.hpp"
#include <iostream>

int main() {
    // log_info("Starting Telemetry Server...");

    SimpleTimer timer(1000);

    int i = 0;
    while (true) {
        if (timer.check()) {
            // Update devices at the polling interval
            
            std::cout << i++ << " Polling...\n";
        }
    }

    return 0;
}
