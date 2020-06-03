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
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_STATUS, gps->fix.status);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_TYPE, gps->fix.type);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_QUALITY, gps->fix.quality);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_ALTITUDE, gps->fix.altitude);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_LATITUDE, gps->fix.latitude);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_LONGITUDE, gps->fix.longitude);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_SPEED, gps->fix.speed);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_TRAVELANGLE, gps->fix.travelAngle);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_TRACKINGSATELLITES, gps->fix.trackingSatellites);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_VISIBLESATELLITES, gps->fix.visibleSatellites);
        SM::getDataCache()->set<float>(DataCache::Key::GPS_FIX_TIMESTAMP_RAWTIME, gps->fix.timestamp.rawTime);
    }
}

void Nmea0183Device::close() {
    delete port;
    delete gps;
    delete timer;
}
