#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>

/*
 * Gets a timestamp as milliseconds since epoch.
 */
inline long long getCurrentMillis() {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

inline std::string getTimestamp() {
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%D %T", timeinfo);
    std::string str(buffer);
    return str;
}

#endif