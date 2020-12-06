#ifndef TELEMETRYSERVER_SERVICE_MANAGER_H
#define TELEMETRYSERVER_SERVICE_MANAGER_H

#include <services/config/data_point_cache.h>
#include <services/logger/logger.h>
#include <services/parser/parser.h>

/**
 * ServiceManager
 * A static class used as a service locator
 * - Creates singleton instances of common services
 * - Provides a way to access these objects anywhere in the project
 */
class ServiceManager {
private:
    static Logger* logger;
    static Parser* parser;

public:
    static void init();
    static void destroy();
    static Logger* getLogger() {
        return logger;
    }
    static Parser* getParser() {
        return parser;
    }
};

// Alias ServiceManager to SM for shorter code
typedef ServiceManager SM;

#endif  // TELEMETRYSERVER_SERVICE_MANAGER_H
