#ifndef H_DEVICE_MANAGER
#define H_DEVICE_MANAGER

#include "device.hpp"
#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <iostream>
#include <libserialport.h>
#include <string>
#include <unordered_map>
using namespace std::chrono;

class DeviceManager {
private:
    std::unordered_map<std::string, Device> deviceList;

public:
    void pollDevices();
    void shutdown();
};

#endif
