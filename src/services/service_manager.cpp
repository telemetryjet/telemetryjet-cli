#include <services/logger/console_logger.h>
#include <services/config/json_env_config.h>
#include <services/database/sqlite_database.h>
#include "service_manager.h"

Config *ServiceManager::config;
Logger *ServiceManager::logger;
Database *ServiceManager::database;

void ServiceManager::init() {
    logger = new ConsoleLogger();
    config = new JsonEnvConfig();
    database = new SqliteDatabase();
}

void ServiceManager::destroy() {
    delete config;
    delete logger;
    delete database;
}