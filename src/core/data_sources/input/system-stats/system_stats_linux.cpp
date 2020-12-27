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

    // Read in all CPU time values from /proc/stat
    std::ifstream procStatFile("/proc/stat");
    procStatFile.ignore(5, ' ');
    std::vector<size_t> cpuTimes;
    for (size_t time; proc_stat >> time; times.push_back(time));

    if (cpuTimes.size() < 4)
        return false;
    float64_t idleTime = cpuTimes[3];
    float64_t totalTime = std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0);
    return true;

    get_cpu_times(idle_time, total_time);

    for (size_t idle_time, total_time; ; sleep(1)) {
        const float idle_time_delta = idle_time - previous_idle_time;
        const float total_time_delta = total_time - previous_total_time;
        const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
        float64_t cpuCurrentProportion
        cpuPreviousIdleTicks = ;
        cpuPreviousTotalTicks = ;
        std::cout << utilization << '%' << std::endl;
        previous_idle_time = idle_time;
        previous_total_time = total_time;
    }
}