#include "csv_data_source.h"

CsvDataSource::CsvDataSource(const std::string& id, const json& options)
    : FileDataSource(id, "csv-file", options) {
    cache = new DataPointCache();
    // TODO: configure timer interval with option
    writeTimer = new SimpleTimer(1000);
    rewriteRequired = true;
}

void CsvDataSource::update() {
    // TODO: check if keys contains a new key, and rewrite file if so
    // TODO: add timestamp column

    // update file on interval
    if (outputFile.is_open() && writeTimer->check()) {
        if(rewriteRequired) {
            headers.empty();
            for (const auto& key : cache->getKeys()) {
                headers.push_back(key);
            }

            // write headers
            std::string headerLine;
            for (const auto& header : headers) {
                headerLine.append(fmt::format(headerLine.empty() ? "{}" : ",{}", header));
            }
            outputFile << fmt::format("{}\n", headerLine);
            rewriteRequired = false;
        }

        // write line
        std::string line;
        for (const auto& header : headers) {
            line.append(fmt::format(line.empty() ? "{}" : ",{}", cache->get(header)->value));
        }
        outputFile << fmt::format("{}\n", line);
    }

    // update cache
    for (auto& inDataPoint : in) {
        cache->set(inDataPoint->key, inDataPoint);
    }

    if (flushTimer) {
        outputFile.flush();
    }
}

CsvDataSource::~CsvDataSource() = default;