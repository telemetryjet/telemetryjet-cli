#include "nmea_0183_file_input.h"
#include "boost/date_time/posix_time/posix_time.hpp"

void NMEA0183FileInputDataSource::open() {
    gps = new nmea::GPSService(parser);
    gps->onUpdate += [&]() {
        // Exit early if we don't have a GPS fix
        if (gps->fix.status != 'A') {
            return;
        }
        uint64_t tsHour = gps->fix.timestamp.hour;
        uint64_t tsMin = gps->fix.timestamp.min;
        float64_t tsSecFloat = gps->fix.timestamp.sec;
        uint64_t tsSec = floor(tsSecFloat);
        uint64_t tsMillis = (tsSecFloat - tsSec) * 1000.0;
        uint64_t tsMonth = gps->fix.timestamp.month;
        uint64_t tsDay = gps->fix.timestamp.day;
        uint64_t tsYear = gps->fix.timestamp.year;

        boost::posix_time::ptime pt(boost::gregorian::date(tsYear, tsMonth, tsDay), boost::posix_time::time_duration(tsHour, tsMin, tsSec, tsMillis));
        uint64_t gpsTimestamp = (pt - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_milliseconds();

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
    FileInputDataSource::open();
}

void NMEA0183FileInputDataSource::close() {
    delete gps;
    FileInputDataSource::close();
}

void NMEA0183FileInputDataSource::update() {
    char inChar;
    bool fileOpen = true;
    if (inputFile.get(inChar)) {
        parser.readByte(inChar);
    } else {
        fileOpen = false;
    }
    if (!fileOpen) {
        state = INACTIVE;
    }
}