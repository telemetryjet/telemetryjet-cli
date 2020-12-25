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
    int writeInterval;
    uint64_t lastTimestamp;

    std::vector<std::string> headers;
    std::unordered_set<std::string> headerSet;
    bool rewriteRequired;
    int newHeaderCount;

    void rewrite();
    static std::string sanitizeString(const std::string& input);
public:
    CsvFileOutputDataSource(const json& definition);
    void open() override;
    void update() override;
};

#endif
