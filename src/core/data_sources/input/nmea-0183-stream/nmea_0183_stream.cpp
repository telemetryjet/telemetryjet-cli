#include "nmea_0183_stream.h"

NMEA0183StreamDataSource::NMEA0183StreamDataSource(const std::string &id, const json &options)
        : SerialStreamDataSource(id, "nmea-0183-stream", options) {
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

void NMEA0183StreamDataSource::open() {
    gps = new nmea::GPSService(parser);
    gps->onUpdate += [&]() {
        uint64_t gpsTimestamp = getCurrentMillis();
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.almanac.averageSNR", id), gpsTimestamp, (float64_t)gps->fix.almanac.averageSNR()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.almanac.minSNR", id), gpsTimestamp, (float64_t)gps->fix.almanac.minSNR()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.almanac.maxSNR", id), gpsTimestamp, (float64_t)gps->fix.almanac.maxSNR()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.almanac.percentComplete", id), gpsTimestamp, (float64_t)gps->fix.almanac.percentComplete()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.status", id), gpsTimestamp, fmt::format("{}",gps->fix.status)));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.type", id), gpsTimestamp, (uint8_t)gps->fix.type));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.quality", id), gpsTimestamp, (uint8_t)gps->fix.quality));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.dilution", id), gpsTimestamp, (float64_t)gps->fix.dilution));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.horizontalDilution", id), gpsTimestamp, (float64_t)gps->fix.horizontalDilution));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.verticalDilution", id), gpsTimestamp, (float64_t)gps->fix.verticalDilution));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.altitude", id), gpsTimestamp, (float64_t)gps->fix.altitude));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.latitude", id), gpsTimestamp, (float64_t)gps->fix.latitude));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.longitude", id), gpsTimestamp, (float64_t)gps->fix.longitude));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.speed", id), gpsTimestamp, (float64_t)gps->fix.speed));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.travelAngle", id), gpsTimestamp, (float64_t)gps->fix.travelAngle));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.trackingSatellites", id), gpsTimestamp, (int32_t)gps->fix.trackingSatellites));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.visibleSatellites", id), gpsTimestamp, (int32_t)gps->fix.visibleSatellites));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.locked", id), gpsTimestamp, (bool_t)gps->fix.locked()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.horizontalAccuracy", id), gpsTimestamp, (float64_t)gps->fix.horizontalAccuracy()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.verticalAccuracy", id), gpsTimestamp, (float64_t)gps->fix.verticalAccuracy()));
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.hasEstimate", id), gpsTimestamp, (bool_t)gps->fix.hasEstimate()));
    };
    parser.log = false;
    SerialStreamDataSource::open();
}

void NMEA0183StreamDataSource::close() {
    delete gps;
    SerialStreamDataSource::close();
}