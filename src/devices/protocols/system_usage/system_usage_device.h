#ifndef TELEMETRYSERVER_SYSTEM_USAGE_DEVICE_H
#define TELEMETRYSERVER_SYSTEM_USAGE_DEVICE_H

#include "devices/device.h"
#include "core/data_sources/system_stats/system_usage_statistics.h"
#include "utility/serial_wrapper.h"
#include <utility/simple_timer.h>

class SystemUsageDevice : public Device {
private:
    SimpleTimer* timer;

public:
    void open(std::string address) override;
    void update() override;
    void close() override;
    std::string getAddress() override;

    static std::string name() {
        return "System Usage";
    }
};

#endif  // TELEMETRYSERVER_SYSTEM_USAGE_DEVICE_H
