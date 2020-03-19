#ifndef TELEMETRYSERVER_DEVICE_MANAGER_H
#define TELEMETRYSERVER_DEVICE_MANAGER_H

#include <set>
#include <string>
#include <unordered_map>
#include "device.h"

class DeviceManager {
private:
    std::unordered_map<std::string, Device*> deviceList;
    std::set<std::string> portList;
    bool isRunning;
public:
    DeviceManager() = default;
    ~DeviceManager();
    void start();
    void update();
    void stop();
};

#endif //TELEMETRYSERVER_DEVICE_MANAGER_H
