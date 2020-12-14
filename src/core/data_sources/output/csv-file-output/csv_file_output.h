#ifndef TELEMETRYJETCLI_CSV_FILE_OUTPUT_H
#define TELEMETRYJETCLI_CSV_FILE_OUTPUT_H

#include "core/data_sources/base/file-output/file_output.h"
#include "core/data_cache.h"
#include "utility/timer/simple_timer.h"
#include <unordered_map>
#include <fstream>

class CsvFileOutputDataSource : public FileOutputDataSource {
private:
    SimpleTimer* writeTimer;
    DataPointCache* cache;

    std::vector<std::string> headers;
    bool rewriteRequired;

public:
    CsvFileOutputDataSource(const std::string& id, const json& options);
    ~CsvFileOutputDataSource() override;
    void update() override;
};

#endif
