#include "in_memory_cache.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <services/service_manager.h>
#include <fmt/format.h>

namespace pt = boost::posix_time;

InMemoryCache::InMemoryCache() {
    ServiceManager::getLogger()->info("Started In-Memory Data Cache.");
}

InMemoryCache::~InMemoryCache() {
    ServiceManager::getLogger()->info("Stopped In-Memory Data Cache.");
}

float InMemoryCache::getFloat(const std::string& key) {
    if (cache.find(key) == cache.end()){
        return 0;
    } else {
        return cache[key];
    }
}

float InMemoryCache::setFloat(const std::string& key, float value) {
    record_log_t::createLog(fmt::format("Set data point [key = {}, value = {}].", key, value));
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();

    if (cache.find(key) == cache.end()){
        cache[key] = value;
        keys.push_back(key);
    }
    cache[key] = value;
    //cache[key].lastUpdated = milliseconds;
    //cache[key].value = value;
    //cache[key].avg_value += value;
    //cache[key].avg_value_quantity += 1;
    //cache[key].max_value = std::max(value, cache[key].max_value);
    //cache[key].min_value = std::min(value, cache[key].min_value);
}

std::vector<std::string> InMemoryCache::getKeys() {
    return keys;
}