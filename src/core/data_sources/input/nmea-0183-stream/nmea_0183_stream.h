#ifndef TELEMETRYJETCLI_NMEA_0183_STREAM_H
#define TELEMETRYJETCLI_NMEA_0183_STREAM_H

#include "core/data_source.h"
#include "core/data_sources/base/serial-stream/serial_stream.h"
#include <nmeaparse/nmea.h>

class NMEA0183StreamDataSource : public SerialStreamDataSource {
private:
    nmea::NMEAParser parser;
    nmea::GPSService* gps;
public:
    NMEA0183StreamDataSource(const json &definition): SerialStreamDataSource(definition) {}
    void open() override;
    void close() override;
    void update() override;
};

#endif