#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>

namespace pt = boost::posix_time;
namespace ch = std::chrono;

/*
 * Gets a timestamp as milliseconds since epoch.
 */
inline long long getCurrentMillis() {
    return duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();
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