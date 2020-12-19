#include "system_stats.h"
#include <utility>
#include <boost/filesystem.hpp>

SystemStatsDataSource::SystemStatsDataSource(std::string id, const json &options)
    : DataSource(std::move(id), "system-stats") {}

SystemStatsDataSource::~SystemStatsDataSource() = default;

void SystemStatsDataSource::open() {
    timer = new SimpleTimer(1000);
}

void SystemStatsDataSource::close() {
    delete timer;
}

void SystemStatsDataSource::update() {
    if (timer->check()) {
        computeDiskAvailable();
        computeMemoryAvailable();
        computeCpuStats();
        computeGpuStats();
    }
}

void SystemStatsDataSource::computeDiskAvailable() {
    uint64_t timestamp = getCurrentMillis();

    boost::filesystem::space_info si = boost::filesystem::space("/");
    uint64_t mDiskCapacity = si.capacity;
    uint64_t mDiskAvailable = si.available;
    uint64_t mDiskUsage = si.capacity - si.available;
    out.push_back(std::make_shared<DataPoint>(fmt::format("{}.disk.capacity", id), timestamp, mDiskCapacity));
    out.push_back(std::make_shared<DataPoint>(fmt::format("{}.disk.available", id), timestamp, mDiskAvailable));
    out.push_back(std::make_shared<DataPoint>(fmt::format("{}.disk.usage", id), timestamp, mDiskUsage));
}