#include <services/logger/ConsoleLogger.h>
#include "ServiceManager.h"

Config *ServiceManager::config;
Logger *ServiceManager::logger;

void ServiceManager::init() {
    logger = new ConsoleLogger();
}

void ServiceManager::destroy() {
    delete logger;
}