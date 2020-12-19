#include "system_stats.h"
#include <cstdio>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/vm_statistics.h>

void SystemStatsDataSource::computeMemoryAvailable() {
    uint64_t timestamp = getCurrentMillis();

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

        uint64_t mPhysicalMemoryAvailable = free_memory;
        uint64_t mPhysicalMemoryUsed = used_memory;
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.available", id), timestamp, mPhysicalMemoryAvailable));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.used", id), timestamp, mPhysicalMemoryUsed));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.ram.total", id), timestamp, mPhysicalMemoryAvailable + mPhysicalMemoryUsed));
    }
}

void SystemStatsDataSource::computeCpuStats() {
    uint64_t timestamp = getCurrentMillis();

    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        uint64_t totalTicks = 0;
        for (int i=0; i<CPU_STATE_MAX; i++) {
            totalTicks += cpuinfo.cpu_ticks[i];
        }
        uint64_t idleTicks = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        uint64_t totalTicksSinceLastTime = totalTicks - cpuPreviousTotalTicks;
        uint64_t idleTicksSinceLastTime  = idleTicks - cpuPreviousIdleTicks;
        float64_t cpuCurrentProportion = 1.0f -((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);
        cpuPreviousTotalTicks = totalTicks;
        cpuPreviousIdleTicks  = idleTicks;
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.cpu", id), timestamp, cpuCurrentProportion * 100.0));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.cpu.idle", id), timestamp, (1.0 - cpuCurrentProportion) * 100.0));
    }
}

void SystemStatsDataSource::computeGpuStats() {
    uint64_t timestamp = getCurrentMillis();
}