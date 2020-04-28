#ifndef TELEMETRYSERVER_CONSOLE_LOGGER_H
#define TELEMETRYSERVER_CONSOLE_LOGGER_H

#include "logger.h"

/**
 * ConsoleLogger
 * An implementation of the logger, which logs to the console.
 */
class ConsoleLogger: public Logger {
private:
    LoggerLevel level = LoggerLevel::LEVEL_HEADER;
public:
    ConsoleLogger();
    ~ConsoleLogger() override;
    void header(std::string message, bool createLogRecord = false) override;
    void debug(std::string message, bool createLogRecord = false) override;
    void info(std::string message, bool createLogRecord = false) override;
    void warning(std::string message, bool createLogRecord = false) override;
    void error(std::string message, bool createLogRecord = false) override;
    void alert(std::string message, bool createLogRecord = false) override;
    void setLevel(LoggerLevel newLevel) override;
    void setLevel(std::string newLevel) override;
    LoggerLevel getLevel() override;
};

#endif //TELEMETRYSERVER_CONSOLE_LOGGER_H
