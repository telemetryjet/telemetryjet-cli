#include <services/ServiceManager.h>

/**
 * Main Program Entry Point
 * Starts and runs the server loop.
 */

int exitCode = 0;
bool running = true;

void signalHandler(int signum) {
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize the common services.
    ServiceManager::init();

    ServiceManager::getLogger()->info("Started Telemetry Server!");

    // Run the server loop
    while (running) {
    }

    ServiceManager::getLogger()->info("Stopped Telemetry Server.");

    // Shutdown the common services
    ServiceManager::destroy();

    // Exit main program
    return exitCode;
}