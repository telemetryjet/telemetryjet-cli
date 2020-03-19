#ifndef TELEMETRYSERVER_LOGGER_H
#define TELEMETRYSERVER_LOGGER_H

#include <string>

/**
 * Logger Level
 */
 enum class LoggerLevel: int {
     LEVEL_HEADER = 1,
     LEVEL_DEBUG = 2,
     LEVEL_INFO = 3,
     LEVEL_WARNING = 4,
     LEVEL_ERROR = 5,
     LEVEL_ALERT = 6,
     LEVEL_NONE = 7
 };

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
    virtual void alert(std::string message) = 0;
    virtual void setLevel(LoggerLevel level) = 0;
    virtual void setLevel(std::string level) = 0;
    virtual LoggerLevel getLevel() = 0;
};

#endif //TELEMETRYSERVER_LOGGER_H
