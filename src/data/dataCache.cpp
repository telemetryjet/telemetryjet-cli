//
// Created by Chris Dalke on 1/29/20.
//

#include "dataCache.h"

DataCache::DataCache() {
    std::cout << "Initialized in-memory data cache...\n";
}

float DataCache::getFloat(const std::string& key){
    if (this->cache.find(key) == this->cache.end()){
        return 0;
    } else {
        return this->cache[key];
    }
}

float DataCache::setFloat(const std::string& key, float value){
    this->cache[key] = value;
}
