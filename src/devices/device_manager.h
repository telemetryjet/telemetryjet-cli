#ifndef TELEMETRYSERVER_DEVICE_MANAGER_H
#define TELEMETRYSERVER_DEVICE_MANAGER_H

#include "device.h"
#include <map>
#include <string>
#include <utility/simple_timer.h>
#include <vector>

class DeviceManager {
private:
    std::vector<Device*> deviceList;
    std::vector<std::string> portList;
    SimpleTimer* timer;
    std::vector<record_device_t> deviceDefinitions;

    enum protocolTypes { NMEA_0183, SYSTEM_USAGE, MOCK_DEVICE, NUM_PROTOCOLS };

public:
    bool isRunning = false;
    DeviceManager();
    ~DeviceManager();
    void start();
    void update();
    void stop();

    static std::map<int, std::string> getProtocolMap();
};

#endif  // TELEMETRYSERVER_DEVICE_MANAGER_H
