#include "console_logger.h"
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

std::string getTimestamp() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%D %T",timeinfo);
    std::string str(buffer);
    return str;
}

void log(const std::string& message, const std::string& level){
    fmt::print("[{}] {}: {}\n", getTimestamp(), level, message);
}
void logColor(ansi_color_code colorCode, const std::string& message, const std::string& level){
    fmt::print("\033[1;{}m[{}] {}: {}\033[0m\n", static_cast<int>(colorCode), getTimestamp(), level, message);
}
void logHeader(ansi_color_code colorCode, const std::string& message){
    fmt::print("\033[1;{}m{}\033[0m\n", static_cast<int>(colorCode), message);
}

void ConsoleLogger::header  (std::string message) { logHeader(ansi_color_code::blue, message); }
void ConsoleLogger::debug   (std::string message) { log(message, "DEBUG"); }
void ConsoleLogger::info    (std::string message) { log(message, "INFO"); }
void ConsoleLogger::warning (std::string message) { logColor(ansi_color_code::yellow, message, "WARNING"); }
void ConsoleLogger::error   (std::string message) { logColor(ansi_color_code::red, message, "ERROR"); }

ConsoleLogger::ConsoleLogger() {
    header("  _ _  ______  __     __");
    header( "  __  /_  __/ / /__  / /_");
    header( "   ___ / /_  / / _ \\/ __/");
    header( "  __  / / /_/ /  __/ /_");
    header( " _ _ /_/\\____/\\___/\\__/");
    header( fmt::format("TelemetryJet Server v{}",TELEMETRYJET_VERSION));
    header( "--------------------------");
    info("Started Console Logger.");
}

ConsoleLogger::~ConsoleLogger() {
    info("Stopped Console Logger.");
}