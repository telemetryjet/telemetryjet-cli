#include "system_usage_device.h"
#include "fmt/format.h"
#include "services/service_manager.h"
#include <utility>

std::string SystemUsageDevice::getAddress() {
    return port->getPortName();
}

void SystemUsageDevice::open(std::string address) {
    port = new SerialWrapper(address, 9600);

    timer = new SimpleTimer(1000);

    stats = new SystemUsageStatistics();

    SM::getLogger()->info(fmt::format("Started " + name() + " device at port = {}", address));
}

void SystemUsageDevice::update() {
    port->poll();

    while (!port->getBuffer().empty()) {
        std::cout << (port->getBuffer().front()) << std::endl;
        port->getBuffer().pop_front();
    }

    // At polling interval, save the data points to the cache
    if (timer->check()) {
        stats->calculate();

        SM::getDataCache()->set<unsigned long>("sys.disk.avail", stats->mDiskAvailable);
        SM::getDataCache()->set<unsigned long>("sys.disk.used", stats->mDiskUsage);
        SM::getDataCache()->set<unsigned long>("sys.mem.avail", stats->mPhysicalMemoryAvailable);
        SM::getDataCache()->set<unsigned long>("sys.mem.used", stats->mPhysicalMemoryUsed);
        SM::getDataCache()->set<float>("sys.cpu.used", stats->mCpuUsage);
        SM::getDataCache()->set<float>("sys.cpu.temp", stats->mCpuTemperature);
        SM::getDataCache()->set<float>("sys.gpu.used", stats->mGpuUsage);
        SM::getDataCache()->set<float>("sys.gpu.temp", stats->mGpuTemperature);

    }
}

void SystemUsageDevice::close() {
    delete port;
    delete timer;
    delete stats;
}
