#include "service_manager.h"

Config* ServiceManager::config;
Logger* ServiceManager::logger;
Parser* ServiceManager::parser;

void ServiceManager::init() {
    config = new Config();
    logger = new Logger();
    parser = new Parser();
}

void ServiceManager::destroy() {
    delete parser;
    delete logger;
    delete config;
}