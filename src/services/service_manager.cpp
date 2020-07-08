#include "service_manager.h"
#include <constants.h>
#include <fmt/format.h>
#include <services/api/rest/rest_api_server.h>
#include <services/config/json_env_config.h>
#include <services/config/persisted_config.h>
#include <services/data_cache/cache.h>
#include <services/database/sqlite/sqlite_database.h>
#include <services/logger/console_logger.h>

Config* ServiceManager::config;
Config* ServiceManager::persistedConfig;
Logger* ServiceManager::logger;
Database* ServiceManager::database;
RestApiServer* ServiceManager::restApiServer;
StreamingServer* ServiceManager::streamingServer;
DeviceManager* ServiceManager::deviceManager;
DataCache* ServiceManager::dataCache;

void ServiceManager::init() {
    logger = new ConsoleLogger();

    // Set log enabled from configuration variable as soon as possible
    char* loggingEnv = getenv("TELEMETRYJET_LOGGING");
    bool setLogDisabledViaEnv = false;
    if (loggingEnv != nullptr && strcmp(loggingEnv, "false") == 0) {
        logger->setLevel(LoggerLevel::LEVEL_NONE);
        setLogDisabledViaEnv = true;
    }

    // Set log level from configuration variable as soon as possible.
    char* logLevelEnv = getenv("TELEMETRYJET_LOG_LEVEL");
    bool setLogLevelViaEnv = false;
    if (logLevelEnv != nullptr) {
        logger->setLevel(logLevelEnv);
        setLogLevelViaEnv = true;
    }

    logger->header("  _ _  ______  __     __");
    logger->header("  __  /_  __/ / /__  / /_");
    logger->header("   ___ / /_  / / _ \\/ __/");
    logger->header("  __  / / /_/ /  __/ /_");
    logger->header(" _ _ /_/\\____/\\___/\\__/");
    logger->header(fmt::format("TelemetryJet Server v{}", TELEMETRYJET_VERSION));
    logger->header("--------------------------");
    logger->info("Started Console Logger.");

    config = new JsonEnvConfig();

    // Set log enabled and log level for logger once full config is loaded
    bool logEnabled = config->getBool("logging", true);
    std::string logLevel = config->getString("log_level", "header");

    if (!setLogDisabledViaEnv && !logEnabled) {
        logger->warning(
            "Disabling logging via JSON configuration applies after startup. \nTo disable startup messages, please specify "
            "this config option with environment variable TELEMETRYJET_LOGGING=true/false.");
        logger->setLevel(LoggerLevel::LEVEL_NONE);
    }

    if (!setLogLevelViaEnv && logEnabled) {
        logger->info(fmt::format("Setting log level to {}", logLevel));
        logger->setLevel(logLevel);
        if (logLevel != "header") {
            logger->warning(
                    "Setting log level via JSON configuration applies after startup. \nTo filter startup messages, please specify "
                    "this config option with environment variable TELEMETRYJET_LOG_LEVEL=...");
        }
    }

    // Setup SQLite database
    logger->debug("ServiceManager: Initializing SQLite Database");
    database = new SqliteDatabase();

    // Set up in-memory data cache
    logger->debug("ServiceManager: Initializing Data Cache");
    dataCache = new DataCache();

    // Set up persisted configuration, which pulls from the database
    logger->debug("ServiceManager: Initializing Persisted Configuration");
    persistedConfig = new PersistedConfig();

    // Set up REST API server
    logger->debug("ServiceManager: Initializing REST API Server");
    restApiServer = new RestApiServer();

    // Setup streaming server
    logger->debug("ServiceManager: Initializing Streaming API Server");
    streamingServer = new StreamingServer();

    // Setup Device Manager
    logger->debug("ServiceManager: Initializing Device Manager");
    deviceManager = new DeviceManager();
}

void ServiceManager::destroy() {
    delete deviceManager;
    delete config;
    delete persistedConfig;
    delete dataCache;
    delete database;
    delete restApiServer;
    delete streamingServer;
    delete logger;
}