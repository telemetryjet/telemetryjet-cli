#include "cache.h"
#include "services/service_manager.h"

Cache::Cache() {
    SM::getLogger()->info("Started In-Memory Data Cache.");
}
Cache::~Cache() {
    SM::getLogger()->info("Stopped In-Memory Data Cache.");
}
