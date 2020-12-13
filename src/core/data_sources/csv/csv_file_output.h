#ifndef TELEMETRYJETCLI_CSV_FILE_OUTPUT_H
#define TELEMETRYJETCLI_CSV_FILE_OUTPUT_H

#include "core/data_cache.h"
#include "core/data_sources/output_file_data_source.h"
#include "utility/timer/simple_timer.h"
#include <fstream>
#include <unordered_map>

class CsvFileOutput : public OutputFileDataSource {
private:
    SimpleTimer* writeTimer;
    DataPointCache* cache;

    std::vector<std::string> headers;
    bool rewriteRequired;

public:
    CsvFileOutput(const std::string& id, const json& options);
    ~CsvFileOutput() override;
    void update() override;
};

#endif
