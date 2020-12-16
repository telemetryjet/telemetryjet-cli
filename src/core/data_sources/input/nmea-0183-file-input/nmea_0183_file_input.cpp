#include "nmea_0183_file_input.h"

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
        uint64_t gpsTimestamp = gps->fix.timestamp.getTime() * 1000;
        createDataPoint("gps_fix_status", gps->fix.status, gpsTimestamp);
        createDataPoint("gps_fix_type", gps->fix.type, gpsTimestamp);
        createDataPoint("gps_fix_quality", gps->fix.quality, gpsTimestamp);
        createDataPoint("gps_fix_altitude", (float)gps->fix.altitude, gpsTimestamp);
        createDataPoint("gps_fix_latitude", (float)gps->fix.latitude, gpsTimestamp);
        createDataPoint("gps_fix_longitude", (float)gps->fix.longitude, gpsTimestamp);
        createDataPoint("gps_fix_speed", (float)gps->fix.speed, gpsTimestamp);
        createDataPoint("gps_fix_travelAngle", (float)gps->fix.travelAngle, gpsTimestamp);
        createDataPoint("gps_fix_trackingSatellites", gps->fix.trackingSatellites, gpsTimestamp);
        createDataPoint("gps_fix_visibleSatellites", gps->fix.visibleSatellites, gpsTimestamp);
    };
    parser.log = false;
    FileInputDataSource::open();
}

void NMEA0183FileInputDataSource::close() {
    delete gps;
    FileInputDataSource::close();
}
void NMEA0183FileInputDataSource::createDataPoint(std::string key, float value, uint64_t timestamp) {
    out.push_back(std::make_shared<DataPoint>(key, value, timestamp));
}
