#ifndef TELEMETRYSERVER_LOGGER_H
#define TELEMETRYSERVER_LOGGER_H

#include <string>

/**
 * Logger
 * Interface for a logger, which should implement 4 logging levels:
 * - Debug, Info, Warning, Error
 */
class Logger {
public:
    virtual ~Logger() = default;
    virtual void header(std::string message) = 0;
    virtual void debug(std::string message) = 0;
    virtual void info(std::string message) = 0;
    virtual void warning(std::string message) = 0;
    virtual void error(std::string message) = 0;
};

#endif //TELEMETRYSERVER_LOGGER_H