#include "nmea_0183_stream.h"

void NMEA0183StreamDataSource::open() {
    gps = new nmea::GPSService(parser);
    gps->onUpdate += [&]() {
        uint64_t gpsTimestamp = getCurrentMillis();
        write(std::make_shared<DataPoint>("almanac.averageSNR", gpsTimestamp, (float64_t)gps->fix.almanac.averageSNR()));
        write(std::make_shared<DataPoint>("almanac.minSNR", gpsTimestamp, (float64_t)gps->fix.almanac.minSNR()));
        write(std::make_shared<DataPoint>("almanac.maxSNR", gpsTimestamp, (float64_t)gps->fix.almanac.maxSNR()));
        write(std::make_shared<DataPoint>("almanac.percentComplete", gpsTimestamp, (float64_t)gps->fix.almanac.percentComplete()));
        write(std::make_shared<DataPoint>("status", gpsTimestamp, fmt::format("{}",gps->fix.status)));
        write(std::make_shared<DataPoint>("type", gpsTimestamp, (uint8_t)gps->fix.type));
        write(std::make_shared<DataPoint>("quality", gpsTimestamp, (uint8_t)gps->fix.quality));
        write(std::make_shared<DataPoint>("dilution", gpsTimestamp, (float64_t)gps->fix.dilution));
        write(std::make_shared<DataPoint>("horizontalDilution", gpsTimestamp, (float64_t)gps->fix.horizontalDilution));
        write(std::make_shared<DataPoint>("verticalDilution", gpsTimestamp, (float64_t)gps->fix.verticalDilution));
        write(std::make_shared<DataPoint>("altitude", gpsTimestamp, (float64_t)gps->fix.altitude));
        write(std::make_shared<DataPoint>("latitude", gpsTimestamp, (float64_t)gps->fix.latitude));
        write(std::make_shared<DataPoint>("longitude", gpsTimestamp, (float64_t)gps->fix.longitude));
        write(std::make_shared<DataPoint>("speed", gpsTimestamp, (float64_t)gps->fix.speed));
        write(std::make_shared<DataPoint>("travelAngle", gpsTimestamp, (float64_t)gps->fix.travelAngle));
        write(std::make_shared<DataPoint>("trackingSatellites", gpsTimestamp, (int32_t)gps->fix.trackingSatellites));
        write(std::make_shared<DataPoint>("visibleSatellites", gpsTimestamp, (int32_t)gps->fix.visibleSatellites));
        write(std::make_shared<DataPoint>("locked", gpsTimestamp, (bool_t)gps->fix.locked()));
        write(std::make_shared<DataPoint>("horizontalAccuracy", gpsTimestamp, (float64_t)gps->fix.horizontalAccuracy()));
        write(std::make_shared<DataPoint>("verticalAccuracy", gpsTimestamp, (float64_t)gps->fix.verticalAccuracy()));
        write(std::make_shared<DataPoint>("hasEstimate", gpsTimestamp, (bool_t)gps->fix.hasEstimate()));
    };
    parser.log = false;
    SerialStreamDataSource::open();
}

void NMEA0183StreamDataSource::close() {
    delete gps;
    SerialStreamDataSource::close();
}

void NMEA0183StreamDataSource::update() {
    SerialStreamDataSource::update();

    if (!serial->getBuffer().empty()) {
        std::string tempLine;
        for (auto& inChar : serial->getBuffer()) {
            parser.readByte(inChar);
        }
        serial->clearBuffer();
    }
}
