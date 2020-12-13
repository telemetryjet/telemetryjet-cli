#ifndef TELEMETRYJETCLI_OUTPUT_FILE_DATA_SOURCE_H
#define TELEMETRYJETCLI_OUTPUT_FILE_DATA_SOURCE_H

#include "core/data_source.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include "utility/timer/simple_timer.h"

class OutputFileDataSource : public DataSource {
protected:
    std::ofstream outputFile;
    std::string filename;
    std::ios::openmode mode;
    std::string modeString;
    bool generateNewFile;
    SimpleTimer* flushTimer;
public:
    OutputFileDataSource(const std::string& id, const std::string& type, const json &options);
    ~OutputFileDataSource() override;
    void open() override;
    void close() override;
    bool isOpen() override;
};

#endif
