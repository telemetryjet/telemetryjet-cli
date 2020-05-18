#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <chrono>
#include <cstdint>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;

/*
 * Gets a timestamp as milliseconds since epoch.
 */
inline long long getCurrentMillis() {
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    return current_date_microseconds.time_of_day().total_milliseconds();
}

inline std::string getTimestamp() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%D %T",timeinfo);
    std::string str(buffer);
    return str;
}

#endif