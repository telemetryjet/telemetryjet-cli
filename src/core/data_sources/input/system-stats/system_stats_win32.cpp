#include "system_stats.h"
#include <windows.h>
/**
 * Implementation from
 * https://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c
 */

void SystemStatsDataSource::computeMemoryAvailable() {
    uint64_t timestamp = getCurrentMillis();

    if (windowsMemInfo == nullptr) {
        windowsMemInfo = malloc(sizeof(MEMORYSTATUSEX));
        windowsMemInfo = new MEMORYSTATUSEX();
        ((MEMORYSTATUSEX*)windowsMemInfo)->dwLength = sizeof(*(MEMORYSTATUSEX*)windowsMemInfo);
        GlobalMemoryStatusEx((MEMORYSTATUSEX*)windowsMemInfo);
    }

    auto* memInfo = (MEMORYSTATUSEX*)windowsMemInfo;
    uint64_t mPhysicalMemoryAvailable = memInfo->ullAvailPhys;
    uint64_t mPhysicalMemoryTotal = memInfo->ullTotalPhys;
    uint64_t mPhysicalMemoryUsed = mPhysicalMemoryTotal - mPhysicalMemoryAvailable;
    write(std::make_shared<DataPoint>(fmt::format("{}.ram.available", id), timestamp, mPhysicalMemoryAvailable));
    write(std::make_shared<DataPoint>(fmt::format("{}.ram.used", id), timestamp, mPhysicalMemoryUsed));
    write(std::make_shared<DataPoint>(fmt::format("{}.ram.total", id), timestamp, mPhysicalMemoryTotal));
}


static uint64_t FileTimeToInt64(const FILETIME & ft) {
    return (((uint64_t)(ft.dwHighDateTime)) << 32) | ((uint64_t)ft.dwLowDateTime);
}

void SystemStatsDataSource::computeCpuStats() {
    uint64_t timestamp = getCurrentMillis();

    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        uint64_t idleTicks = FileTimeToInt64(idleTime);
        uint64_t totalTicks = FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime);
        uint64_t totalTicksSinceLastTime = totalTicks - cpuPreviousTotalTicks;
        uint64_t idleTicksSinceLastTime = idleTicks - cpuPreviousIdleTicks;
        float64_t cpuCurrentProportion = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);
        cpuPreviousTotalTicks = totalTicks;
        cpuPreviousIdleTicks  = idleTicks;
        write(std::make_shared<DataPoint>(fmt::format("{}.cpu", id), timestamp, cpuCurrentProportion * 100.0));
        write(std::make_shared<DataPoint>(fmt::format("{}.cpu.idle", id), timestamp, (1.0 - cpuCurrentProportion) * 100.0));
    }
}