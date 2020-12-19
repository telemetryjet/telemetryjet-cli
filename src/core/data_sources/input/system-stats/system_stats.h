#ifndef SYSTEM_STATS_DATA_SOURCE_H
#define SYSTEM_STATS_DATA_SOURCE_H

#include "core/data_source.h"
#include "utility/timer/simple_timer.h"

class SystemStatsDataSource : public DataSource {
private:
    SimpleTimer* timer;
    void computeDiskAvailable();
    void computeMemoryAvailable();
    void computeCpuStats();
    void computeGpuStats();
    uint64_t cpuPreviousTotalTicks = 0;
    uint64_t cpuPreviousIdleTicks = 0;
public:
    explicit SystemStatsDataSource(std::string id, const json &options);
    ~SystemStatsDataSource() override;
    void open() override;
    void close() override;
    void update() override;
};

#endif