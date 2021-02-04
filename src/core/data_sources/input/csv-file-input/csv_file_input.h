#ifndef TELEMETRYJETCLI_CSV_FILE_INPUT_H
#define TELEMETRYJETCLI_CSV_FILE_INPUT_H

#include "core/data_sources/base/file-input/file_input.h"

class CsvFileInputDataSource : public FileInputDataSource {
private:
    std::vector<std::string> headers;
    bool hasTimestamp;
public:
    CsvFileInputDataSource(const json& definition) : FileInputDataSource(definition) {}
    void open() override;
    void update() override;
};

#endif  // TELEMETRYJETCLI_CSV_FILE_INPUT_H
