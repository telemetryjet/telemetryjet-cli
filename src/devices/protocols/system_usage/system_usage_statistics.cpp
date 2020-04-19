#include "system_usage_statistics.h"
#include <boost/filesystem.hpp>
#include <cstdio>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/vm_statistics.h>

SystemUsageStatistics::SystemUsageStatistics() {
    int nErr = getCpuInfo(&mSystem, &mUser, &mNice, &mIdle);
    if (nErr != KERN_SUCCESS) {
        printf("Kernel error: %s\n", mach_error_string(nErr));
    }
}

void SystemUsageStatistics::calculate() {

    // disk usage statistics
    boost::filesystem::space_info si = boost::filesystem::space("/");
    mDiskAvailable = si.available;
    mDiskUsage = si.capacity - si.available;

    // physical memory statistics
    vm_size_t page_size;
    vm_statistics64_data_t vm_stats;
    mach_port_t mach_port = mach_host_self();
    mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
    if (KERN_SUCCESS == host_page_size(mach_port, &page_size)
        && KERN_SUCCESS
            == host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count)) {
        long long free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;

        long long used_memory = ((int64_t)vm_stats.active_count + (int64_t)vm_stats.inactive_count
                                 + (int64_t)vm_stats.wire_count)
            * (int64_t)page_size;

        mPhysicalMemoryAvailable = free_memory;
        mPhysicalMemoryUsed = used_memory;
    }

    // cpu usage statistics
    unsigned long systemPrev = mSystem;
    unsigned long userPrev = mUser;
    unsigned long nicePrev = mNice;
    unsigned long idlePrev = mIdle;

    int nErr = getCpuInfo(&mSystem, &mUser, &mNice, &mIdle);
    if (nErr == KERN_SUCCESS) {
        float fUsageTime = (float)(mSystem - systemPrev) + (mUser - userPrev) + (mNice - nicePrev);
        float fTotalTime = fUsageTime + (float)(mIdle - idlePrev);
        mCpuUsage = fUsageTime / fTotalTime * 100;
    } else {
        printf("Kernel error: %s\n", mach_error_string(nErr));
    }
}

int SystemUsageStatistics::getCpuInfo(unsigned long* pulSystem,
                                      unsigned long* pulUser,
                                      unsigned long* pulNice,
                                      unsigned long* pulIdle) {
    mach_msg_type_number_t unCpuMsgCount = 0;
    processor_flavor_t nCpuFlavor = PROCESSOR_CPU_LOAD_INFO;
    kern_return_t nErr = 0;
    natural_t unCPUNum = 0;
    processor_cpu_load_info_t structCpuData;
    host_t host = mach_host_self();
    *pulSystem = 0;
    *pulUser = 0;
    *pulNice = 0;
    *pulIdle = 0;
    nErr = host_processor_info(host,
                               nCpuFlavor,
                               &unCPUNum,
                               (processor_info_array_t*)&structCpuData,
                               &unCpuMsgCount);
    for (int i = 0; i < (int)unCPUNum; i++) {
        *pulSystem += structCpuData[i].cpu_ticks[CPU_STATE_SYSTEM];
        *pulUser += structCpuData[i].cpu_ticks[CPU_STATE_USER];
        *pulNice += structCpuData[i].cpu_ticks[CPU_STATE_NICE];
        *pulIdle += structCpuData[i].cpu_ticks[CPU_STATE_IDLE];
    }
    return nErr;
}