#ifndef H_DEVICE_MANAGER
#define H_DEVICE_MANAGER

#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <iostream>
#include <libserialport.h>
#include <string>
using namespace std::chrono;

class DeviceManager {
private:
public:
    void pollDevices();
};

#endif
