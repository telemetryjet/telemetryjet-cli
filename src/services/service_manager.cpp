#include "service_manager.h"

Config* ServiceManager::config;
Logger* ServiceManager::logger;

void ServiceManager::init() {
    config = new Config();
    logger = new Logger();
}

void ServiceManager::destroy() {
    delete logger;
    delete config;
}