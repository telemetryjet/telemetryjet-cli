#include "service_manager.h"
#include <constants.h>
#include <fmt/format.h>
#include <services/config/json_env_config.h>
#include <services/config/persisted_config.h>
#include <services/data_cache/cache.h>
#include <services/database/sqlite/sqlite_database.h>
#include <services/logger/console_logger.h>

Config* ServiceManager::config;
Logger* ServiceManager::logger;
Database* ServiceManager::database;
DataCache* ServiceManager::dataCache;

void ServiceManager::init() {
    logger = new ConsoleLogger();

    // TODO: decide what to do with logging, hardcoding to log level none for now
    logger->setLevel(LoggerLevel::LEVEL_NONE);

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

    config = new JsonEnvConfig();

    // Set log enabled and log level for logger once full config is loaded
    bool logEnabled = config->getBool("logging", false);
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
}

void ServiceManager::destroy() {
    delete config;
    delete dataCache;
    delete database;
    delete logger;
}