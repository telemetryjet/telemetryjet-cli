#ifndef TELEMETRYSERVER_NMEA_0183_DEVICE_H
#define TELEMETRYSERVER_NMEA_0183_DEVICE_H

#include "devices/device.h"
#include "utility/serial/serial_wrapper.h"
#include <nmeaparse/nmea.h>
#include <utility/timer/simple_timer.h>

using namespace nmea;

class Nmea0183Device : public Device {
private:
    SerialWrapper* port;
    std::list<uint8_t> stringBuffer;
    SimpleTimer* timer;

    // NMEA parsing
    NMEAParser parser;
    GPSService* gps;

public:
    void open(std::string address) override;
    void update() override;
    void close() override;
    std::string getAddress() override;

    static std::string name() {
        return "NMEA 0183";
    }
};

#endif  // TELEMETRYSERVER_NMEA_0183_DEVICE_H