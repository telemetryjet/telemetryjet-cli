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

        SM::getDataCache()->set<uint64_t>("sys.disk.avail", stats->mDiskAvailable);
        SM::getDataCache()->set<uint64_t>("sys.disk.used", stats->mDiskUsage);
        SM::getDataCache()->set<uint64_t>("sys.mem.avail", stats->mPhysicalMemoryAvailable);
        SM::getDataCache()->set<uint64_t>("sys.mem.used", stats->mPhysicalMemoryUsed);
        SM::getDataCache()->set<float>("sys.cpu.used", stats->mCpuUsage);
        SM::getDataCache()->set<float>("sys.cpu.temp", stats->mCpuTemperature);
        SM::getDataCache()->set<float>("sys.gpu.used", stats->mGpuUsage);
        SM::getDataCache()->set<float>("sys.gpu.temp", stats->mGpuTemperature);
    }
}

void SystemUsageDevice::close() {
    delete timer;
    delete stats;
}
