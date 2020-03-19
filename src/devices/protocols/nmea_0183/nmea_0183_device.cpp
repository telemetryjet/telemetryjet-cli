#include "nmea_0183_device.h"
#include "services/service_manager.h"
#include "fmt/format.h"
#include <cstdint>
#include <utility>

void Nmea0183Device::open(std::string address) {
    port = new SerialWrapper(address, 9600);

    gps = new GPSService(parser);
    parser.log = false;

    timer = new SimpleTimer(100);

    SM::getLogger()->info(fmt::format("Started NMEA 0183 device at port = {}", address));
}

void Nmea0183Device::update()  {
    port->poll();

    while (!port->getBuffer().empty()) {
        parser.readByte(port->getBuffer().front());
        port->getBuffer().pop_front();
    }

    // TODO: At polling interval, save the data points to the cache
    if (timer->check()) {

    }
}

void Nmea0183Device::close()  {
    delete port;
    delete gps;
    delete timer;
}
