#include "system_usage_device.h"
#include "fmt/format.h"
#include "services/service_manager.h"
#include <utility>

std::string SystemUsageDevice::getAddress() {
    return "dummy_address";
}

void SystemUsageDevice::open(std::string address) {
    SM::getLogger()->info(fmt::format("Started " + name() + " device at port = {}", address));
}

void SystemUsageDevice::update() {
}

void SystemUsageDevice::close() {
    delete timer;
    delete stats;
}
