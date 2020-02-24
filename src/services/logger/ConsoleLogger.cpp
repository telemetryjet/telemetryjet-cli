#include "ConsoleLogger.h"
#include <fmt/format.h>

std::string getTimestamp() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%D %T",timeinfo);
    std::string str(buffer);
    return str;
}

void log(const std::string& message, const std::string& level){
    fmt::print("[{}] {}: {}\n", getTimestamp(), level, message);
}
void logErr(const std::string& message, const std::string& level){
    fmt::print(stderr,"[{}] {}: {}\n", getTimestamp(), level, message);
}

void ConsoleLogger::debug   (std::string message) { log(message, "DEBUG"); }
void ConsoleLogger::info    (std::string message) { log(message, "INFO"); }
void ConsoleLogger::warning (std::string message) { log(message, "WARNING"); }
void ConsoleLogger::error   (std::string message) { logErr(message, "ERROR"); }

ConsoleLogger::ConsoleLogger() {
    error("Initialized Console Logger.");
}

ConsoleLogger::~ConsoleLogger() {
    info("Shutdown Console Logger.");
}
