#ifndef TELEMETRYJETCLI_CSV_FILE_INPUT_H
#define TELEMETRYJETCLI_CSV_FILE_INPUT_H

#include "utility/time_utils.h"
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
    TimestampUnits timestampUnits = TimestampUnits::MILLISECONDS;
    uint64_t startTimestamp = 0;
    int lineCount = 0;
    int cellCount = 0;

    inline static const std::string FIRST_LINE_IS_HEADER_KEY = "firstLineIsHeader";
    inline static const std::string SEPARATOR_KEY = "separator";
    inline static const std::string TIMESTAMP_COLUMN_NAME_KEY = "timestampColumnName";
    inline static const std::string HEADERS_KEY = "headers";
    inline static const std::string TIMESTAMP_TYPE_KEY = "timestampType";
    inline static const std::string START_TIMESTAMP_KEY = "startTimestamp";
    inline static const std::string TIMESTAMP_UNITS_KEY = "timestampUnits";

    void parseCsvSpecificOptions(const json& definition);
public:
    explicit CsvFileInputDataSource(const json& definition);
    void open() override;
    void update() override;
};

#endif  // TELEMETRYJETCLI_CSV_FILE_INPUT_H
