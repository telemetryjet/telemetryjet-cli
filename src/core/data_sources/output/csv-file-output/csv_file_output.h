#ifndef TELEMETRYJETCLI_CSV_FILE_OUTPUT_H
#define TELEMETRYJETCLI_CSV_FILE_OUTPUT_H

#include "core/data_cache.h"
#include "core/data_sources/base/file-output/file_output.h"
#include "utility/timer/simple_timer.h"
#include <fstream>
#include <unordered_map>

class CsvFileOutputDataSource : public FileOutputDataSource {
private:
    SimpleTimer* writeTimer;
    DataPointCache* cache;

    std::vector<std::string> headers;
    std::unordered_set<std::string> headerSet;
    bool rewriteRequired;
    int newHeaderCount;

    void rewrite();
public:
    CsvFileOutputDataSource(const std::string& id, const json& options);
    void update() override;
};

#endif
