//
// Created by chris on 12/4/2020.
//

#include "system_stats.h"

SystemStatsDataSource::SystemStatsDataSource(std::string id) : DataSource(id, "system_stats") {

}

SystemStatsDataSource::~SystemStatsDataSource() {

}

void SystemStatsDataSource::open() {
    timer = new SimpleTimer(1000);
    stats = new SystemUsageStatistics();
}

void SystemStatsDataSource::close() {
    delete timer;
    delete stats;
}

void SystemStatsDataSource::update() {
    if (timer->check()) {
        stats->calculate();
        uint64_t timestamp = getCurrentMillis();
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_disk_avail", id), stats->mDiskAvailable, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_disk_used", id), stats->mDiskUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_mem_avail", id), stats->mPhysicalMemoryAvailable, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_mem_used", id), stats->mPhysicalMemoryUsed, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_cpu_used", id), stats->mCpuUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_cpu_temp", id), stats->mCpuTemperature, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_gpu_used", id), stats->mGpuUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_gpu_temp", id), stats->mGpuTemperature, timestamp));
    }
}

bool SystemStatsDataSource::isOpen() {
    return false;
}

