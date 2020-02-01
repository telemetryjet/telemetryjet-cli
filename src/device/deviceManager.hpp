#ifndef H_DEVICE_MANAGER
#define H_DEVICE_MANAGER

#include "device.hpp"
#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <iostream>
#include <libserialport.h>
#include <set>
#include <string>
#include <unordered_map>
using namespace std::chrono;

class DeviceManager {
private:
    std::unordered_map<std::string, Device> deviceList;
    std::set<std::string>                   portList;
    bool                                    _paused;
    void                                    disconnectDevices();
    void                                    log(std::string message);

public:
    void update();
    void getData();
    void shutdown();
    void setPaused(bool paused);
    DeviceManager();
};

#endif
