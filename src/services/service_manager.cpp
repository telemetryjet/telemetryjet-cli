#include "service_manager.h"

Logger* ServiceManager::logger;
Parser* ServiceManager::parser;

void ServiceManager::init() {
    logger = new Logger();
    parser = new Parser();
}

void ServiceManager::destroy() {
    delete parser;
    delete logger;
}