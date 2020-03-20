#include <services/logger/console_logger.h>
#include <services/config/json_env_config.h>
#include <services/config/persisted_config.h>
#include <services/database/sqlite/sqlite_database.h>
#include <services/data_cache/in_memory_cache.h>
#include <fmt/format.h>
#include <constants.h>
#include <services/api/rest/rest_api_server.h>
#include "service_manager.h"

Config *ServiceManager::config;
Config *ServiceManager::persistedConfig;
Logger *ServiceManager::logger;
Database *ServiceManager::database;
RestApiServer *ServiceManager::restApiServer;
DeviceManager *ServiceManager::deviceManager;
Cache *ServiceManager::dataCache;

void ServiceManager::init() {
    logger = new ConsoleLogger();

    // Set log enabled from configuration variable as soon as possible
    char *loggingEnv = getenv("TELEMETRYJET_LOGGING");
    if (loggingEnv != nullptr && strcmp(loggingEnv, "false") == 0){
        logger->setLevel(LoggerLevel::LEVEL_NONE);
    }

    // Set log level from configuration variable as soon as possible.
    char *logLevelEnv = getenv("TELEMETRYJET_LOG_LEVEL");
    bool setLogLevelViaEnv = false;
    if (logLevelEnv != nullptr){
        logger->setLevel(logLevelEnv);
        setLogLevelViaEnv = true;
    }

    logger->header("  _ _  ______  __     __");
    logger->header( "  __  /_  __/ / /__  / /_");
    logger->header( "   ___ / /_  / / _ \\/ __/");
    logger->header( "  __  / / /_/ /  __/ /_");
    logger->header( " _ _ /_/\\____/\\___/\\__/");
    logger->header( fmt::format("TelemetryJet Server v{}",TELEMETRYJET_VERSION));
    logger->header( "--------------------------");
    logger->info("Started Console Logger.");

    config = new JsonEnvConfig();

    // Set log enabled for logger once full config is loaded
    bool logEnabled = config->getBool("logging", true);
    if (!logEnabled) {
        logger->warning("Disabled logging via JSON configuration. \nTo filter startup messages, please specify this config option with environment variable TELEMETRYJET_LOGGING=false.");
        logger->setLevel(LoggerLevel::LEVEL_NONE);
    }

    if (!setLogLevelViaEnv && logEnabled) {
        std::string logLevel = config->getString("log_level","header");
        logger->info(fmt::format("Setting log level to {}", logLevel));
        logger->warning("Set log level via JSON configuration. \nTo filter startup messages, please specify this config option with environment variable TELEMETRYJET_LOG_LEVEL=...");
        logger->setLevel(logLevel);
    }

    // Setup SQLite database
    database = new SqliteDatabase();

    // Set up in-memory data cache
    dataCache = new InMemoryCache();

    // Set up persisted configuration, which pulls from the database
    persistedConfig = new PersistedConfig();

    // Set up REST API server
    restApiServer = new RestApiServer();

    // Setup Device Manager
    deviceManager = new DeviceManager();
}

void ServiceManager::destroy() {
    delete deviceManager;
    delete config;
    delete persistedConfig;
    delete logger;
    delete dataCache;
    delete database;
    delete restApiServer;
}