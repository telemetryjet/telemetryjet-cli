#ifndef TELEMETRYJETCLI_SERIAL_STREAM_BASE_H
#define TELEMETRYJETCLI_SERIAL_STREAM_BASE_H

#include "core/data_source.h"
#include "utility/serial/serial_wrapper.h"
#include "utility/timer/simple_timer.h"

class SerialStreamDataSource : public DataSource {
protected:
    std::string portName;
    int baudRate;
    std::unique_ptr<SerialWrapper> serial;
    std::unique_ptr<SimpleTimer> reconnectTimer;
public:
    SerialStreamDataSource(const json &definition);
    void open() override;
    void close() override;
    void update() override;
};

#endif