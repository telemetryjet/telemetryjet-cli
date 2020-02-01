#ifndef device_hpp
#define device_hpp

#include "simpleTimer.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <libserialport.h>
#include <list>
#include <string>
using namespace std::chrono;

class Device {
private:
    std::string        _portName;
    sp_port*           _port;
    bool               _isOpen;
    std::list<uint8_t> buffer;
    SimpleTimer        ppsTimer;
    int                numPackets;

public:
    Device(std::string portName);
    bool isOpen() const {
        return _isOpen;
    }
    void getData();
    void close();
};

#endif
