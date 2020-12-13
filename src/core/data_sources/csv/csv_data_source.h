#ifndef TELEMETRYJETCLI_CSV_DATA_SOURCE_H
#define TELEMETRYJETCLI_CSV_DATA_SOURCE_H

#include "core/data_sources/file_data_source.h"
#include "core/data_cache.h"
#include "utility/timer/simple_timer.h"
#include <unordered_map>
#include <fstream>

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
