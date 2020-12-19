#include "nmea_0183_file_input.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

NMEA0183FileInputDataSource::NMEA0183FileInputDataSource(const std::string& id,
                                                         const json& options)
    : FileInputDataSource(id,"nmea-0183-file-input",options) {
}
void NMEA0183FileInputDataSource::update() {
    DataSource::update();
    if (isOpen) {
        char inChar;
        bool fileOpen = true;
        if (inputFile.get(inChar)) {
            parser.readByte(inChar);
        } else {
            fileOpen = false;
        }
        if (!fileOpen) {
            isDoneReading = true;
        }
    }
}
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
    FileInputDataSource::open();
}

void NMEA0183FileInputDataSource::close() {
    delete gps;
    FileInputDataSource::close();
}