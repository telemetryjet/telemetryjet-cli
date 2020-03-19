#ifndef TELEMETRYSERVER_DEVICE_MANAGER_H
#define TELEMETRYSERVER_DEVICE_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <utility/simple_timer.h>
#include "device.h"

class DeviceManager {
private:
    std::vector<Device*> deviceList;
    std::vector<std::string> portList;
    bool isRunning = false;
    SimpleTimer *timer;
public:
    DeviceManager();
    ~DeviceManager();
    void start();
    void update();
    void stop();
};

#endif //TELEMETRYSERVER_DEVICE_MANAGER_H
