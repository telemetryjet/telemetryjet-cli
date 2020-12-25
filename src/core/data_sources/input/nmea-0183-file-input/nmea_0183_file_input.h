#ifndef TELEMETRYJETCLI_NMEA_0183_FILE_INPUT_H
#define TELEMETRYJETCLI_NMEA_0183_FILE_INPUT_H

#include "core/data_sources/base/file-input/file_input.h"
#include <nmeaparse/nmea.h>

class NMEA0183FileInputDataSource : public FileInputDataSource {
private:
    nmea::NMEAParser parser;
    nmea::GPSService* gps;
public:
    NMEA0183FileInputDataSource(const json &definition): FileInputDataSource(definition) {}
    void open() override;
    void close() override;
    void update() override;
};

#endif