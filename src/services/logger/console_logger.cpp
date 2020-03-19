#include "console_logger.h"
#include "utility/time_utils.h"
#include <fmt/format.h>
#include <constants.h>

// ANSI color code from
// https://gist.github.com/zyvitski/fb12f2ce6bc9d3b141f3bd4410a6f7cf
// https://en.wikipedia.org/wiki/ANSI_escape_code
enum class ansi_color_code: int{
    black = 30,
    red=31,
    green=32,
    yellow=33,
    blue=34,
    magenta=35,
    cyan=36,
    white=37,
    bright_black=90,
    bright_red=91,
    bright_green=92,
    bright_yellow=93,
    bright_blue=94,
    bright_magenta=95,
    bright_cyan=96,
    bright_white=97,
};

void log(const std::string& message, const std::string& level){
    fmt::print("[{}] {}: {}\n", getTimestamp(), level, message);
}

void logColor(ansi_color_code colorCode, const std::string& message, const std::string& level){
    fmt::print("\033[1;{}m[{}] {}: {}\033[0m\n", static_cast<int>(colorCode), getTimestamp(), level, message);
}
void logHeader(ansi_color_code colorCode, const std::string& message){
    fmt::print("\033[1;{}m{}\033[0m\n", static_cast<int>(colorCode), message);
}

void ConsoleLogger::header  (std::string message) {
    if (level <= LoggerLevel::LEVEL_HEADER) {
        logHeader(ansi_color_code::blue, message);
    }
}

void ConsoleLogger::debug   (std::string message) {
    if (level <= LoggerLevel::LEVEL_DEBUG) {
        log(message, "DEBUG");
    }
}

void ConsoleLogger::info    (std::string message) {
    if (level <= LoggerLevel::LEVEL_INFO) {
        log(message, "INFO");
    }
}

void ConsoleLogger::warning (std::string message) {
    if (level <= LoggerLevel::LEVEL_WARNING) {
        logColor(ansi_color_code::yellow, message, "WARNING");
    }
}

void ConsoleLogger::error   (std::string message) {
    if (level <= LoggerLevel::LEVEL_ERROR) {
        logColor(ansi_color_code::red, message, "ERROR");
    }
}

void ConsoleLogger::alert(std::string message) {
    if (level <= LoggerLevel::LEVEL_ALERT) {
        logColor(ansi_color_code::green, message, "ALERT");
    }
}

ConsoleLogger::ConsoleLogger() = default;

ConsoleLogger::~ConsoleLogger() {
    info("Stopped Console Logger.");
}

LoggerLevel ConsoleLogger::getLevel() {
    return level;
}

void ConsoleLogger::setLevel(LoggerLevel newLevel) {
    level = newLevel;
}

void ConsoleLogger::setLevel(std::string newLevel) {
    if (newLevel == "header") { level = LoggerLevel::LEVEL_HEADER; }
    else if (newLevel == "debug") { level = LoggerLevel::LEVEL_DEBUG; }
    else if (newLevel == "info") { level = LoggerLevel::LEVEL_INFO; }
    else if (newLevel == "warning") { level = LoggerLevel::LEVEL_WARNING; }
    else if (newLevel == "error") { level = LoggerLevel::LEVEL_ERROR; }
    else if (newLevel == "none") { level = LoggerLevel::LEVEL_NONE; }
    else {
        error(fmt::format("Unknown log level {}, options are ( header | debug | info | warning | error | none )",newLevel));
    }
}