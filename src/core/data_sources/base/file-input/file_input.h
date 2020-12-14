#ifndef TELEMETRYJETCLI_FILE_INPUT_BASE_H
#define TELEMETRYJETCLI_FILE_INPUT_BASE_H

#include "core/data_source.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include "utility/timer/simple_timer.h"

class FileInputDataSource : public DataSource {
protected:
    std::ifstream inputFile;
    std::string filename;
public:
    FileInputDataSource(const std::string& id, const std::string& type, const json &options);
    ~FileInputDataSource() override;
    void open() override;
    void close() override;
    bool isOpen() override;
};

#endif
