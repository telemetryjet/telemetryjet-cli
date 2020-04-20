#include "cache.h"
#include "services/service_manager.h"

DataCache::DataCache() {
    SM::getLogger()->info("Started In-Memory Data Cache.");
}
DataCache::~DataCache() {
    SM::getLogger()->info("Stopped In-Memory Data Cache.");
}
