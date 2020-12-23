#include "logger.h"
#include <fmt/format.h>
#include <iostream>

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

void log(const std::string& message, const std::string& level) {
    //fmt::print("[{}] {}: {}\n", getTimestamp(), level, message);
    fmt::print(stderr, "{}\n", message);
}

void Logger::clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cerr << "\033[2J\033[H";
}

void logColor(ansi_color_code colorCode, const std::string& message, const std::string& level) {
    /*fmt::print("\033[1;{}m[{}] {}: {}\033[0m\n",
               static_cast<int>(colorCode),
               getTimestamp(),
               level,
               message);
               */
    fmt::print(stderr, "\033[1;{}m{}\033[0m\n",
               static_cast<int>(colorCode),
               message);
    fflush(stderr);
}
void logHeader(ansi_color_code colorCode, const std::string& message) {
    fmt::print("\033[1;{}m{}\033[0m\n", static_cast<int>(colorCode), message);
    fflush(stderr);
}

void Logger::header(std::string message) {
    if (level <= LoggerLevel::LEVEL_HEADER) {
        logHeader(ansi_color_code::blue, message);
    }
}

void Logger::debug(std::string message) {
    if (level <= LoggerLevel::LEVEL_DEBUG) {
        log(message, "DEBUG");
    }
}

void Logger::info(std::string message) {
    if (level <= LoggerLevel::LEVEL_INFO) {
        log(message, "INFO");
    }
}

void Logger::warning(std::string message) {
    if (level <= LoggerLevel::LEVEL_WARNING) {
        logColor(ansi_color_code::yellow, message, "WARNING");
    }
}

void Logger::error(std::string message) {
    if (level <= LoggerLevel::LEVEL_ERROR) {
        logColor(ansi_color_code::red, message, "ERROR");
    }
}

void Logger::alert(std::string message) {
    if (level <= LoggerLevel::LEVEL_ALERT) {
        logColor(ansi_color_code::green, message, "ALERT");
    }
}

Logger::Logger() = default;

Logger::~Logger() = default;

LoggerLevel Logger::getLevel() {
    return level;
}

void Logger::setLevel(LoggerLevel newLevel) {
    level = newLevel;
}

void Logger::setLevel(std::string newLevel) {
    if (newLevel == LOG_LEVEL_HEADER) {
        level = LoggerLevel::LEVEL_HEADER;
    } else if (newLevel == LOG_LEVEL_DEBUG) {
        level = LoggerLevel::LEVEL_DEBUG;
    } else if (newLevel == LOG_LEVEL_INFO) {
        level = LoggerLevel::LEVEL_INFO;
    } else if (newLevel == LOG_LEVEL_WARNING) {
        level = LoggerLevel::LEVEL_WARNING;
    } else if (newLevel == LOG_LEVEL_ERROR) {
        level = LoggerLevel::LEVEL_ERROR;
    } else if (newLevel == LOG_LEVEL_NONE) {
        level = LoggerLevel::LEVEL_NONE;
    } else {
        error(fmt::format(
            "Unknown log level {}, options are ( header | debug | info | warning | error | none )",
            newLevel));
    }
}
