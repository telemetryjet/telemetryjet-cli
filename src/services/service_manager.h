#ifndef TELEMETRYSERVER_SERVICE_MANAGER_H
#define TELEMETRYSERVER_SERVICE_MANAGER_H

#include <services/logger/logger.h>
#include <services/config/config.h>
#include <services/database/database.h>
#include <services/api/rest/rest_api_server.h>

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
    static Database *database;
    static RestApiServer *restApiServer;
public:
    static void init();
    static void destroy();
    static Config* getConfig() { return config; }
    static Logger* getLogger() { return logger; }
    static Database* getDatabase() { return database; }
    static RestApiServer* getRestApiServer() { return restApiServer; }
};


// Alias ServiceManager to SM for shorter code
typedef ServiceManager SM;

#endif //TELEMETRYSERVER_SERVICE_MANAGER_H
