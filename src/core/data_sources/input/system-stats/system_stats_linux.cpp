#include "system_stats.h"
#include <sys/sysinfo.h>

void SystemStatsDataSource::computeMemoryAvailable() {
    uint64_t timestamp = getCurrentMillis();

    struct sysinfo infoObj;
    if (sysinfo(&infoObj) == 0) {
        uint64_t totalRam = infoObj.totalram;
        uint64_t usedRam = infoObj.totalram - infoObj.freeram;
        uint64_t freeRam = infoObj.freeram;
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.available", id), timestamp, freeRam));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.used", id), timestamp, usedRam));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.total", id), timestamp, totalRam));
    }
}

void SystemStatsDataSource::computeCpuStats() {
    uint64_t timestamp = getCurrentMillis();

}