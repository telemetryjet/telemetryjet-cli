#ifndef TELEMETRYSERVER_SERVICEMANAGER_H
#define TELEMETRYSERVER_SERVICEMANAGER_H

#include <services/logger/Logger.h>
#include <services/config/Config.h>

/**
 * ServiceManager
 * A static class used as a service locator
 * - Creates singleton instances of common services
 * - Provides a way to access these objects anywhere in the project
 */
class ServiceManager {
private:
    static Config *config;
    static Logger *logger;
public:
    static void init();
    static void destroy();
    static Config* getConfig() { return config; }
    static Logger* getLogger() { return logger; }
};

#endif //TELEMETRYSERVER_SERVICEMANAGER_H
