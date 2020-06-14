#include "system_usage_statistics.h"
#include <boost/filesystem.hpp>
#include <cstdio>

SystemUsageStatistics::SystemUsageStatistics() {
}

void SystemUsageStatistics::calculate() {
    // disk usage statistics
    boost::filesystem::space_info si = boost::filesystem::space("/");
    mDiskAvailable = si.available;
    mDiskUsage = si.capacity - si.available;
}

int SystemUsageStatistics::getCpuInfo(unsigned long* pulSystem,
                                      unsigned long* pulUser,
                                      unsigned long* pulNice,
                                      unsigned long* pulIdle) {
}