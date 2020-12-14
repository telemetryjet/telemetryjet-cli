#ifndef TELEMETRYJETCLI_SERIAL_STREAM_BASE_H
#define TELEMETRYJETCLI_SERIAL_STREAM_BASE_H

#include "core/data_source.h"

class SerialStreamDataSource : public DataSource {
protected:
public:
    SerialStreamDataSource(const std::string &id, const std::string &type, const json &options);
    ~SerialStreamDataSource() override;
    void open() override;
    void close() override;
    bool isOpen() override;
};

#endif