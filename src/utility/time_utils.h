#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <ctime>
#include <iostream>
#include <chrono>
#include <locale>

/*
 * Gets a timestamp as milliseconds since epoch.
 */
inline long long getCurrentMillis() {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

inline std::string getTimestamp() {
    std::time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, sizeof(buffer), "%m/%d/%y %H:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
}

#endif