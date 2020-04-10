#include <services/service_manager.h>
#include <utility/time_utils.h>
#include <fmt/format.h>
#include <model/records.h>
#include <tray.h>

/**
 * Main Program Entry Point
 * Starts and runs the server loop.
 */

int exitCode = 0;
bool running = true;

void signalHandler(int signum) {
    running = false;
}

void quit_cb(struct tray_menu *item) {
    running = false;
}

struct tray tray = {
        .icon = "icon.png",
        .menu = (struct tray_menu[]){{"Test", 0, 0, NULL, NULL},
                                     {"-", 0, 0, NULL, NULL},
                                     {"Quit", 0, 0, quit_cb, NULL},
                                     {NULL, 0, 0, NULL, NULL}},
};

int main() {
    long long startInit = getCurrentMillis();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize the common services.
    ServiceManager::init();

    // Initialize tray icon
    tray_init(&tray);

    // Get the active system, and write some basic data about the setup stats.
    record_system_t activeSystem = record_system_t::getActiveSystem();
    SM::getLogger()->info(fmt::format("Active System: [id={},name={}]", activeSystem.id, activeSystem.name));
    record_system_t::startSystem();

    // Start the device manager if the system is set to be running.
    if (record_system_t::isSystemRunning()) {
        SM::getDeviceManager()->start();
    }

    long long elapsedInitTime = getCurrentMillis() - startInit;

    ServiceManager::getLogger()->info(fmt::format("Started Telemetry Server in {} ms.", elapsedInitTime));

    // Run the server loop
    // TODO: Implement throttling, for now this is handled in the services
    while (running) {
        SM::getDeviceManager()->update();

        tray_loop(0);
    }

    // Stop the device manager, clearing up connections to serial devices.
    SM::getDeviceManager()->stop();

    ServiceManager::getLogger()->info("Stopping Telemetry Server...");

    // Shutdown the common services
    ServiceManager::destroy();

    // Remove tray icon.
    tray_exit();

    // Exit main program
    return exitCode;
}