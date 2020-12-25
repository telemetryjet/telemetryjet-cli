#include "system_stats.h"
#include <boost/filesystem.hpp>

void SystemStatsDataSource::open() {
    timer = std::make_shared<SimpleTimer>(1000);
}

void SystemStatsDataSource::close() {
    timer.reset();
}

void SystemStatsDataSource::update() {
    timer->wait();
    computeDiskAvailable();
    computeMemoryAvailable();
    computeCpuStats();
}

void SystemStatsDataSource::computeDiskAvailable() {
    uint64_t timestamp = getCurrentMillis();

    boost::filesystem::space_info si = boost::filesystem::space("/");
    uint64_t mDiskCapacity = si.capacity;
    uint64_t mDiskAvailable = si.available;
    uint64_t mDiskUsage = si.capacity - si.available;
    write(std::make_shared<DataPoint>("disk.capacity", timestamp, mDiskCapacity));
    write(std::make_shared<DataPoint>("disk.available", timestamp, mDiskAvailable));
    write(std::make_shared<DataPoint>("disk.usage", timestamp, mDiskUsage));
}