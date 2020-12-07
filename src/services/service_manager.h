#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <services/config/config.h>
#include <services/logger/logger.h>

/**
 * ServiceManager
 * A static class used as a service locator
 * - Creates singleton instances of common services
 * - Provides a way to access these objects anywhere in the project
 */
class ServiceManager {
private:
    static Config* config;
    static Logger* logger;

public:
    static void init();
    static void destroy();
    static Config* getConfig() {
        return config;
    }
    static Logger* getLogger() {
        return logger;
    }
};

// Alias ServiceManager to SM for shorter code
typedef ServiceManager SM;

#endif