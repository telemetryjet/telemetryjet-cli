#ifndef TELEMETRYSERVER_CONSOLE_LOGGER_H
#define TELEMETRYSERVER_CONSOLE_LOGGER_H

#include "logger.h"

/**
 * ConsoleLogger
 * An implementation of the logger, which logs to the console.
 */
class ConsoleLogger: public Logger {
public:
    ConsoleLogger();
    ~ConsoleLogger() override;
    void header(std::string message) override;
    void debug(std::string message) override;
    void info(std::string message) override;
    void warning(std::string message) override;
    void error(std::string message) override;
};

#endif //TELEMETRYSERVER_CONSOLE_LOGGER_H
