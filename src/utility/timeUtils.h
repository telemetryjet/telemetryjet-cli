#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;

long long getCurrentMillis() {
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    return current_date_microseconds.time_of_day().total_milliseconds();
}

#endif