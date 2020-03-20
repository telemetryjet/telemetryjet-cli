#ifndef TELEMETRYSERVER_DEVICE_H
#define TELEMETRYSERVER_DEVICE_H

#include <string>

class Device {
public:
    virtual ~Device() = default;
    virtual void open(std::string address) = 0;
    virtual void update() = 0;
    virtual void close() = 0;
    virtual std::string getAddress() = 0;
};

#endif //TELEMETRYSERVER_DEVICE_H
