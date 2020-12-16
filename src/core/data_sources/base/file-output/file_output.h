#ifndef TELEMETRYJETCLI_FILE_OUTPUT_BASE_H
#define TELEMETRYJETCLI_FILE_OUTPUT_BASE_H

#include "core/data_source.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include "utility/timer/simple_timer.h"

class FileOutputDataSource : public DataSource {
protected:
    std::ofstream outputFile;
    std::string filename;
    std::ios::openmode mode;
    std::string modeString;
    bool generateNewFile;
    std::unique_ptr<SimpleTimer> flushTimer;
public:
    FileOutputDataSource(const std::string& id, const std::string& type, const json &options);
    void open() override;
    void close() override;
    bool checkDone() override;
    bool checkExitOnError() override;
};

#endif
