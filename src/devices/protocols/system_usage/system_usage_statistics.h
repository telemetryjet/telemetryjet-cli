#ifndef TELEMETRYSERVER_SYSTEM_USAGE_STATISTICS_H
#define TELEMETRYSERVER_SYSTEM_USAGE_STATISTICS_H

class SystemUsageStatistics {

public:
    SystemUsageStatistics();
    void calculate();

    unsigned long mDiskUsage = 0;
    unsigned long mDiskAvailable = 0;
    unsigned long mPhysicalMemoryUsed = 0;
    unsigned long mPhysicalMemoryAvailable = 0;
    float mCpuUsage = 0;
    float mCpuTemperature = 0;
    float mGpuUsage = 0;
    float mGpuTemperature = 0;

private:
    static int getCpuInfo(unsigned long* pulSystem,
                          unsigned long* pulUser,
                          unsigned long* pulNice,
                          unsigned long* pulIdle);

    unsigned long mSystem;
    unsigned long mUser;
    unsigned long mNice;
    unsigned long mIdle;
};

#endif  // TELEMETRYSERVER_SYSTEM_USAGE_STATISTICS_H
