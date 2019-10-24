#include "device.hpp"

// Opens/initialized the device.
Device::Device(std::string portName)
    : _portName(portName) {
    std::cout << "Initializing device with portName=" << portName << "\n";
    sp_return serialStatus = sp_get_port_by_name(_portName.c_str(), &_port);
    if (serialStatus == SP_OK) {
        serialStatus = sp_open(_port, SP_MODE_READ_WRITE);
        if (serialStatus == SP_OK) {
            sp_set_baudrate(_port, 9600);
            _isOpen = true;
        }
    }
}

// Closes the device.
void Device::close() {
    std::cout << "Closing device with portName=" << _portName << "\n";
    sp_close(_port);
    _isOpen = false;
}