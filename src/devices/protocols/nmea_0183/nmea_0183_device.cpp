#include "nmea_0183_device.h"
#include "services/service_manager.h"
#include "fmt/format.h"
#include <cstdint>
#include <utility>

std::string Nmea0183Device::getAddress() {
    return port->getPortName();
}

void Nmea0183Device::open(std::string address) {
    port = new SerialWrapper(address, 9600);

    gps = new GPSService(parser);
    parser.log = false;

    timer = new SimpleTimer(1000);

    SM::getLogger()->info(fmt::format("Started NMEA 0183 device at port = {}", address));
}

void Nmea0183Device::update()  {
    port->poll();

    while (!port->getBuffer().empty()) {
        parser.readByte(port->getBuffer().front());
        port->getBuffer().pop_front();
    }

    // At polling interval, save the data points to the cache
    if (timer->check()) {
        SM::getDataCache()->setFloat("gps.fix.status", gps->fix.status);
        SM::getDataCache()->setFloat("gps.fix.type", gps->fix.type);
        SM::getDataCache()->setFloat("gps.fix.quality", gps->fix.quality);
        SM::getDataCache()->setFloat("gps.fix.altitude", gps->fix.altitude);
        SM::getDataCache()->setFloat("gps.fix.latitude", gps->fix.latitude);
        SM::getDataCache()->setFloat("gps.fix.longitude", gps->fix.longitude);
        SM::getDataCache()->setFloat("gps.fix.speed", gps->fix.speed);
        SM::getDataCache()->setFloat("gps.fix.travelAngle", gps->fix.travelAngle);
        SM::getDataCache()->setFloat("gps.fix.trackingSatellites", gps->fix.trackingSatellites);
        SM::getDataCache()->setFloat("gps.fix.visibleSatellites", gps->fix.visibleSatellites);
        SM::getDataCache()->setFloat("gps.fix.timestamp.rawTime", gps->fix.timestamp.rawTime);
    }
}

void Nmea0183Device::close()  {
    delete port;
    delete gps;
    delete timer;
}
