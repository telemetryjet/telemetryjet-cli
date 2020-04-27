#include <services/service_manager.h>
#include <utility/time_utils.h>
#include <fmt/format.h>
#include <model/records.h>
#include <ui/tray/tray_ui.h>
#include <utility/simple_timer.h>

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
    long long startInit = getCurrentMillis();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize the common services.
    ServiceManager::init();

    // Initialize tray UI
    //TrayUI::init();

    // Get the active system, and write some basic data about the setup stats.
    record_system_t activeSystem = record_system_t::getActiveSystem();
    SM::getLogger()->info(fmt::format("Active System: [id={},name={}]", activeSystem.id, activeSystem.name));
    record_system_t::startSystem();

    // Start the device manager if the system is set to be running.
    if (record_system_t::isSystemRunning()) {
        SM::getDeviceManager()->start();
    }

    // Initialize timer for periodic saving of data cache to database.
    int databaseSaveInterval = SM::getPersistedConfig()->getInt("dbSaveInterval", 1000);
    SimpleTimer timer(databaseSaveInterval);

    long long elapsedInitTime = getCurrentMillis() - startInit;
    ServiceManager::getLogger()->info(fmt::format("Started Telemetry Server in {} ms.", elapsedInitTime));

    // Run the server loop
    // TODO: Implement throttling, for now this is handled in the services
    while (running) {
        SM::getDeviceManager()->update();

        // Update tray UI
        //TrayUI::update();
        //if (TrayUI::shouldQuit) {
        //    running = false;
        //}

        // Save from data cache to database at the configured interval
        // Data points are connected with each other through a "data frame".
        if (timer.check()) {
            // TODO: Create data frame
            // TODO: Create data point entries for data frame
            // TODO: Send updated data points over websockets to telemetry system
        }
    }

    // Stop the device manager, clearing up connections to serial devices.
    SM::getDeviceManager()->stop();

    ServiceManager::getLogger()->info("Stopping Telemetry Server...");

    // Shutdown the common services
    ServiceManager::destroy();

    // Remove tray UI
    //TrayUI::destroy();

    // Exit main program
    return exitCode;
}