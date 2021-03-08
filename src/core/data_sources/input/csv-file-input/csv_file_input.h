#ifndef TELEMETRYJETCLI_CSV_FILE_INPUT_H
#define TELEMETRYJETCLI_CSV_FILE_INPUT_H

#include "core/data_sources/base/file-input/file_input.h"

class CsvFileInputDataSource : public FileInputDataSource {
private:
    std::vector<std::string> headers;
    bool hasTimestamp;
    int lineCount = 0;
    int cellCount = 0;
    bool firstLineIsHeader = true;
    std::string separator = ",";
    std::string timestampColumnName = "timestamp";
    bool hasRelativeTimestamp = false;
    uint64_t startTimestamp;
    std::string timestampUnits = "milliseconds";

    void parseCsvSpecificOptions(const json& definition);
public:
    explicit CsvFileInputDataSource(const json& definition);
    void open() override;
    void update() override;
};

#endif  // TELEMETRYJETCLI_CSV_FILE_INPUT_H
