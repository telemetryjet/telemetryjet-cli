#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <string>
#include <mutex>

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

// ANSI color code from
// https://gist.github.com/zyvitski/fb12f2ce6bc9d3b141f3bd4410a6f7cf
// https://en.wikipedia.org/wiki/ANSI_escape_code
enum class ansi_color_code : int {
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37,
    bright_black = 90,
    bright_red = 91,
    bright_green = 92,
    bright_yellow = 93,
    bright_blue = 94,
    bright_magenta = 95,
    bright_cyan = 96,
    bright_white = 97,
};

/**
 * Logger
 * A common interface for logging data to the console.
 * Allows log filtering, disabling, color-coded formats.
 */
class Logger {
private:
    LoggerLevel level = LoggerLevel::LEVEL_HEADER;
    void _logHeader(ansi_color_code colorCode, const std::string& message);
    void _logColor(ansi_color_code colorCode, const std::string& message, const std::string& level);
    void _log(const std::string& message, const std::string& level);
    void _logRawLine(std::string line);
    std::mutex logMutex;
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
    void logRawLine(std::string message);
    LoggerLevel getLevel();
};

#endif
