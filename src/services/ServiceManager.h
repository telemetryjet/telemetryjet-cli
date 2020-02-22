#ifndef TELEMETRYSERVER_SERVICEMANAGER_H
#define TELEMETRYSERVER_SERVICEMANAGER_H


#include <services/cache/Cache.h>
#include <services/database/Database.h>
#include <services/deviceManager/DeviceManager.h>
#include <services/logger/ConsoleLogger.h>
#include <services/socketServer/SocketServer.h>
#include <services/webServer/WebServer.h>
#include <services/config/Config.h>
#include <services/cache/InMemoryCache.h>
#include <services/database/sqlite/SqliteDatabase.h>

/**
 * ServiceManager
 * A static class used as a service locator
 * - Creates singleton instances of common services
 * - Provides a way to access these objects anywhere in the project
 */
class ServiceManager {
private:
    static inline ConsoleLogger logger = ConsoleLogger();
    //static Config config;
    static inline InMemoryCache cache = InMemoryCache();
    static inline SqliteDatabase database = SqliteDatabase("test.db");
    //static DeviceManager deviceManager;
    //static SocketServer socketServer;
    //static WebServer webServer;
public:
    static void init();
    static void destroy();
    //static Config& getConfig() { return config; }
    static Cache& getCache() { return cache; }
    static Database& getDatabase() { return database; }
    //static DeviceManager& getDeviceManager() { return deviceManager; }
    static Logger& getLogger() { return logger; }
    //static SocketServer& getSocketServer() { return socketServer; }
    //static WebServer& getWebServer() { return webServer; }
};

#endif //TELEMETRYSERVER_SERVICEMANAGER_H
