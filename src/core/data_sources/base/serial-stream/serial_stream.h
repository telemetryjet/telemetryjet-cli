#ifndef TELEMETRYJETCLI_SERIAL_STREAM_BASE_H
#define TELEMETRYJETCLI_SERIAL_STREAM_BASE_H

#include "core/data_source.h"
#include "utility/serial/serial_wrapper.h"
#include "utility/timer/simple_timer.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>

class SerialStreamDataSource : public DataSource {
protected:
    std::string portName;
    int baudRate;
    std::unique_ptr<SerialWrapper> serial;
    std::unique_ptr<SimpleTimer> reconnectTimer;

    bool rawFileOut = false;
    std::ofstream outputFile;
    std::string filename;
    std::ios::openmode mode;
    std::string modeString = "new";
    std::unique_ptr<SimpleTimer> flushTimer;
public:
    SerialStreamDataSource(const json &definition);
    void open() override;
    void close() override;
    void update() override;
};

#endif