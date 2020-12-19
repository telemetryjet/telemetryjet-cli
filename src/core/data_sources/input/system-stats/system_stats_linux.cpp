#include "system_stats.h"
#include <sys/sysinfo.h>

void SystemStatsDataSource::computeMemoryAvailable() {
    uint64_t timestamp = getCurrentMillis();

    sysinfo info;
    if (sysinfo(&info) == 0) {
        uint64_t totalRam = info.totalram;
        uint64_t usedRam = info.totalram - info.freeram;
        uint64_t freeRam = info.freeram;
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.available", id), timestamp, freeRam));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.used", id), timestamp, usedRam));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.total", id), timestamp, totalRam));
    }
}

void SystemStatsDataSource::computeCpuStats() {
    uint64_t timestamp = getCurrentMillis();

}

void SystemStatsDataSource::computeGpuStats() {
    uint64_t timestamp = getCurrentMillis();

}