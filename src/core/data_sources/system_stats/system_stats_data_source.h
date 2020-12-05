#ifndef TELEMETRYJETCLI_SYSTEM_STATS_DATA_SOURCE_H
#define TELEMETRYJETCLI_SYSTEM_STATS_DATA_SOURCE_H

#include "core/data_source.h"
#include "utility/simple_timer.h"
#include "system_usage_statistics.h"

class SystemStatsDataSource : public DataSource {
private:
    SimpleTimer* timer;
    SystemUsageStatistics* stats;
public:
    SystemStatsDataSource(std::string id);
    ~SystemStatsDataSource() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif //TELEMETRYJETCLI_SYSTEM_STATS_DATA_SOURCE_H
