#ifndef TELEMETRYSERVER_NMEA_0183_DEVICE_H
#define TELEMETRYSERVER_NMEA_0183_DEVICE_H

#include "devices/device.h"
#include "utility/serial_wrapper.h"
#include <nmeaparse/nmea.h>
#include <utility/simple_timer.h>

using namespace nmea;

class Nmea0183Device : public Device {
private:
    SerialWrapper *port;
    std::list<uint8_t> stringBuffer;
    SimpleTimer *timer;

    // NMEA parsing
    NMEAParser parser;
    GPSService *gps;
    void parseNmeaLine(std::string line);
public:
    void open(std::string address) override;
    void update() override;
    void close() override;
};


#endif //TELEMETRYSERVER_NMEA_0183_DEVICE_H
