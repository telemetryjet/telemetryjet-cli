#ifndef TELEMETRYSERVER_SERVICE_MANAGER_H
#define TELEMETRYSERVER_SERVICE_MANAGER_H

#include <services/logger/logger.h>
#include <services/config/config.h>

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

#endif //TELEMETRYSERVER_SERVICE_MANAGER_H
