//
// Created by chris on 12/4/2020.
//

#include "system_stats_data_source.h"

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
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_disk_avail", prefix), stats->mDiskAvailable, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_disk_used", prefix), stats->mDiskUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_mem_avail", prefix), stats->mPhysicalMemoryAvailable, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_mem_used", prefix), stats->mPhysicalMemoryUsed, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_cpu_used", prefix), stats->mCpuUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_cpu_temp", prefix), stats->mCpuTemperature, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_gpu_used", prefix), stats->mGpuUsage, timestamp));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.sys_gpu_temp", prefix), stats->mGpuTemperature, timestamp));
    }
}

bool SystemStatsDataSource::isOpen() {
    return false;
}

