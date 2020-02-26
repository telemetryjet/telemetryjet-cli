#include <services/logger/console_logger.h>
#include <services/config/json_env_config.h>
#include "service_manager.h"

Config *ServiceManager::config;
Logger *ServiceManager::logger;

void ServiceManager::init() {
    logger = new ConsoleLogger();
    config = new JsonEnvConfig();
}

void ServiceManager::destroy() {
    delete config;
    delete logger;
}