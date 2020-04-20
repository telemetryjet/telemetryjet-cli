#ifndef TELEMETRYSERVER_SERVICE_MANAGER_H
#define TELEMETRYSERVER_SERVICE_MANAGER_H

#include <devices/device_manager.h>
#include <services/api/rest/rest_api_server.h>
#include <services/api/streaming/streaming_server.h>
#include <services/config/config.h>
#include <services/data_cache/cache.h>
#include <services/database/database.h>
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
    static Config* persistedConfig;
    static Logger* logger;
    static Database* database;
    static RestApiServer* restApiServer;
    static StreamingServer* streamingServer;
    static DeviceManager* deviceManager;
    static DataCache* dataCache;

public:
    static void init();
    static void destroy();
    static Config* getConfig() {
        return config;
    }
    static Config* getPersistedConfig() {
        return persistedConfig;
    }
    static Logger* getLogger() {
        return logger;
    }
    static Database* getDatabase() {
        return database;
    }
    static RestApiServer* getRestApiServer() {
        return restApiServer;
    }
    static StreamingServer* getStreamingServer() {
        return streamingServer;
    }
    static DeviceManager* getDeviceManager() {
        return deviceManager;
    }
    static DataCache* getDataCache() {
        return dataCache;
    }
};

// Alias ServiceManager to SM for shorter code
typedef ServiceManager SM;

#endif  // TELEMETRYSERVER_SERVICE_MANAGER_H
