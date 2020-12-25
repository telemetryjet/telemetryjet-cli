#ifndef SYSTEM_STATS_DATA_SOURCE_H
#define SYSTEM_STATS_DATA_SOURCE_H

#include "core/data_source.h"
#include "utility/timer/simple_timer.h"

class SystemStatsDataSource : public DataSource {
private:
    std::shared_ptr<SimpleTimer> timer;
    void computeDiskAvailable();
    void computeMemoryAvailable();
    void computeCpuStats();
    uint64_t cpuPreviousTotalTicks = 0;
    uint64_t cpuPreviousIdleTicks = 0;
    void* windowsMemInfo = nullptr;
public:
    SystemStatsDataSource(const json &definition): DataSource(definition) {}
    void open() override;
    void close() override;
    void update() override;
};

#endif