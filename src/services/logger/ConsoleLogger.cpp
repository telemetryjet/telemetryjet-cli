#include "ConsoleLogger.h"
#include <iostream>
#include <boost/log/sources/logger.hpp>

void ConsoleLogger::debug(std::string message) {
    std::cout << message << "\n";
}

void ConsoleLogger::info(std::string message) {
    std::cout << message << "\n";
}

void ConsoleLogger::warning(std::string message) {
    std::cout << message << "\n";
}

void ConsoleLogger::error(std::string message) {
    std::cout << message << "\n";
}

ConsoleLogger::ConsoleLogger() {
    info("Initialized Console Logger.");
}

ConsoleLogger::~ConsoleLogger() {
    info("Shutdown Console Logger.");
}
