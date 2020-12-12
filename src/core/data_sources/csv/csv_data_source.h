#ifndef TELEMETRYJETCLI_CSV_DATA_SOURCE_H
#define TELEMETRYJETCLI_CSV_DATA_SOURCE_H

#include "core/data_sources/file_data_source.h"
#include "core/data_cache.h"
#include "utility/timer/simple_timer.h"
#include <unordered_map>
#include <fstream>

class CsvDataSource : public FileDataSource {
private:
    SimpleTimer* writeTimer;
    DataPointCache* cache;

    std::vector<std::string> headers;
    bool rewriteRequired;

public:
    CsvDataSource(const std::string& id, const json& options);
    ~CsvDataSource() override;
    void update() override;
};

#endif
