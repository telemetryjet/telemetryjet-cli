#ifndef TELEMETRYJETCLI_CSV_FILE_INPUT_H
#define TELEMETRYJETCLI_CSV_FILE_INPUT_H

#include "core/data_sources/base/file-input/file_input.h"

class CsvFileInputDataSource : public FileInputDataSource {
private:
    bool isFirstLineHeader = true;
    std::vector<std::string> headers;
    std::string separator = ",";
    bool hasTimestamp = false;
    bool hasRelativeTimestamp = false;
    std::string timestampColumnName = "timestamp";
    int timestampColumn = 0;
    std::string timestampUnits = "milliseconds";
    uint64_t startTimestamp = 0;
    int lineCount = 0;
    int cellCount = 0;

    void parseCsvSpecificOptions(const json& definition);
public:
    explicit CsvFileInputDataSource(const json& definition);
    void open() override;
    void update() override;
};

#endif  // TELEMETRYJETCLI_CSV_FILE_INPUT_H
