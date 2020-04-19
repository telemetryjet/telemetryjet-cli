#include "nmea_0183_device.h"
#include "fmt/format.h"
#include "services/service_manager.h"
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

void Nmea0183Device::update() {
    port->poll();

    while (!port->getBuffer().empty()) {
        parser.readByte(port->getBuffer().front());
        port->getBuffer().pop_front();
    }

    // At polling interval, save the data points to the cache
    if (timer->check()) {
        SM::getDataCache()->set<float>("gps.fix.status", gps->fix.status);
        SM::getDataCache()->set<float>("gps.fix.type", gps->fix.type);
        SM::getDataCache()->set<float>("gps.fix.quality", gps->fix.quality);
        SM::getDataCache()->set<float>("gps.fix.altitude", gps->fix.altitude);
        SM::getDataCache()->set<float>("gps.fix.latitude", gps->fix.latitude);
        SM::getDataCache()->set<float>("gps.fix.longitude", gps->fix.longitude);
        SM::getDataCache()->set<float>("gps.fix.speed", gps->fix.speed);
        SM::getDataCache()->set<float>("gps.fix.travelAngle", gps->fix.travelAngle);
        SM::getDataCache()->set<float>("gps.fix.trackingSatellites", gps->fix.trackingSatellites);
        SM::getDataCache()->set<float>("gps.fix.visibleSatellites", gps->fix.visibleSatellites);
        SM::getDataCache()->set<float>("gps.fix.timestamp.rawTime", gps->fix.timestamp.rawTime);
    }
}

void Nmea0183Device::close() {
    delete port;
    delete gps;
    delete timer;
}
