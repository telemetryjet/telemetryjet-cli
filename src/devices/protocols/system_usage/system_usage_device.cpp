#include "system_usage_device.h"
#include "fmt/format.h"
#include "services/service_manager.h"
#include <utility>

std::string SystemUsageDevice::getAddress() {
    return "dummy_address";
}

void SystemUsageDevice::open(std::string address) {
    timer = new SimpleTimer(1000);
    stats = new SystemUsageStatistics();
    SM::getLogger()->info(fmt::format("Started " + name() + " device at port = {}", address));
}

void SystemUsageDevice::update() {

    // At polling interval, save the data points to the cache
    if (timer->check()) {
        stats->calculate();

        SM::getDataCache()->set<uint64_t>(DataCache::Key::SYS_DISK_AVAIL, stats->mDiskAvailable);
        SM::getDataCache()->set<uint64_t>(DataCache::Key::SYS_DISK_USED, stats->mDiskUsage);
        SM::getDataCache()->set<uint64_t>(DataCache::Key::SYS_MEM_AVAIL, stats->mPhysicalMemoryAvailable);
        SM::getDataCache()->set<uint64_t>(DataCache::Key::SYS_MEM_USED, stats->mPhysicalMemoryUsed);
        SM::getDataCache()->set<float>(DataCache::Key::SYS_CPU_USED, stats->mCpuUsage);
        SM::getDataCache()->set<float>(DataCache::Key::SYS_CPU_TEMP, stats->mCpuTemperature);
        SM::getDataCache()->set<float>(DataCache::Key::SYS_GPU_USED, stats->mGpuUsage);
        SM::getDataCache()->set<float>(DataCache::Key::SYS_GPU_TEMP, stats->mGpuTemperature);
    }
}

void SystemUsageDevice::close() {
    delete timer;
    delete stats;
}
