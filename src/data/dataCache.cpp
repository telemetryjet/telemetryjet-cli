//
// Created by Chris Dalke on 1/29/20.
//

#include "dataCache.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <algorithm>

namespace pt = boost::posix_time;

DataCache::DataCache() {
    std::cout << "Initialized in-memory data cache...\n";
}

float DataCache::getFloat(const std::string& key){
    if (cache.find(key) == cache.end()){
        return 0;
    } else {
        return cache[key].value;
    }
}

float DataCache::setFloat(const std::string& key, float value){
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

    if (cache.find(key) == cache.end()){
        data_key_t valueObj = {
            -1,
            -1,
            key,
            value,
            0,
            0,
            0,
            0,
            false
        };
        cache[key] = valueObj;
        keys.push_back(key);
    }
    cache[key].lastUpdated = milliseconds;
    cache[key].value = value;
    cache[key].avg_value += value;
    cache[key].avg_value_quantity += 1;
    cache[key].max_value = std::max(value, cache[key].max_value);
    cache[key].min_value = std::min(value, cache[key].min_value);
}

std::vector<std::string> DataCache::getKeys() {
    return keys;
}
