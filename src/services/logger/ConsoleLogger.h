#ifndef TELEMETRYSERVER_CONSOLELOGGER_H
#define TELEMETRYSERVER_CONSOLELOGGER_H

#include "Logger.h"

/**
 * ConsoleLogger
 * An implementation of the logger, which logs to the console.
 */
class ConsoleLogger: public Logger {
public:
    ConsoleLogger();
    ~ConsoleLogger();
    void debug(std::string message) override;
    void info(std::string message) override;
    void warning(std::string message) override;
    void error(std::string message) override;
};

#endif //TELEMETRYSERVER_CONSOLELOGGER_H
