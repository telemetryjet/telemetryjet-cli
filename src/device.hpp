#ifndef device_hpp
#define device_hpp

#include <iostream>
#include <libserialport.h>
#include <string>

class Device {
private:
    std::string _portName;
    sp_port*    _port;
    bool        _isOpen;

public:
    Device(std::string portName);
    bool isOpen() { return _isOpen; }
    void close();
};

#endif
