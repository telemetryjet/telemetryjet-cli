#include "logger.h"
#include <fmt/format.h>
#include <iostream>


void Logger::_logRawLine(std::string line) {
    logMutex.lock();
    std::cout << line;
    logMutex.unlock();
}

void Logger::_log(const std::string& message, const std::string& level) {
    logMutex.lock();
    //fmt::print("[{}] {}: {}\n", getTimestamp(), level, message);
    fmt::print(stderr, "{}\n", message);
    logMutex.unlock();
}

void Logger::clearScreen() {
    logMutex.lock();
    std::cout << "\033[2J\033[H";
    std::cerr << "\033[2J\033[H";
    logMutex.unlock();
}

void Logger::_logColor(ansi_color_code colorCode, const std::string& message, const std::string& level) {
    logMutex.lock();
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
    logMutex.unlock();
}
void Logger::_logHeader(ansi_color_code colorCode, const std::string& message) {
    logMutex.lock();
    fmt::print("\033[1;{}m{}\033[0m\n", static_cast<int>(colorCode), message);
    fflush(stderr);
    logMutex.unlock();
}

void Logger::header(std::string message) {
    if (level <= LoggerLevel::LEVEL_HEADER) {
        _logHeader(ansi_color_code::blue, message);
    }
}

void Logger::debug(std::string message) {
    if (level <= LoggerLevel::LEVEL_DEBUG) {
        _log(message, "DEBUG");
    }
}

void Logger::info(std::string message) {
    if (level <= LoggerLevel::LEVEL_INFO) {
        _log(message, "INFO");
    }
}

void Logger::warning(std::string message) {
    if (level <= LoggerLevel::LEVEL_WARNING) {
        _logColor(ansi_color_code::yellow, message, "WARNING");
    }
}

void Logger::error(std::string message) {
    if (level <= LoggerLevel::LEVEL_ERROR) {
        _logColor(ansi_color_code::red, message, "ERROR");
    }
}

void Logger::alert(std::string message) {
    if (level <= LoggerLevel::LEVEL_ALERT) {
        _logColor(ansi_color_code::green, message, "ALERT");
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
