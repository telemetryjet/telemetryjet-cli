#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <string>

// log level strings
#define LOG_LEVEL_HEADER "header"
#define LOG_LEVEL_DEBUG "debug"
#define LOG_LEVEL_INFO "info"
#define LOG_LEVEL_WARNING "warning"
#define LOG_LEVEL_ERROR "error"
#define LOG_LEVEL_ALERT "alert"
#define LOG_LEVEL_NONE "none"

/**
 * Logger Level
 */
enum class LoggerLevel : int {
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
 * A common interface for logging data to the console.
 * Allows log filtering, disabling, color-coded formats.
 */
class Logger {
private:
    LoggerLevel level = LoggerLevel::LEVEL_HEADER;
public:
    Logger();
    ~Logger();
    void header(std::string message);
    void debug(std::string message);
    void info(std::string message);
    void warning(std::string message);
    void error(std::string message);
    void alert(std::string message);
    void setLevel(LoggerLevel level);
    void setLevel(std::string level);
    void clearScreen();
    LoggerLevel getLevel();
};

#endif
