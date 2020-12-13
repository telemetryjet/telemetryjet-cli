#include "csv_file_output.h"
#include <string>

CsvFileOutput::CsvFileOutput(const std::string& id, const json& options)
    : OutputFileDataSource(id, "csv-file-output", options) {
    cache = new DataPointCache();
    rewriteRequired = true;

    if (!options.contains("write_interval_ms")) {
        std::string interval;
        try {
            interval = options["write_interval_ms"];
            writeTimer = new SimpleTimer(std::stoi(interval));
        } catch (std::exception &e) {
            throw std::runtime_error(fmt::format("Error creating timer with parsed write_interval_ms '{}'. {}", interval, e.what()));
        }
    } else {
        writeTimer = new SimpleTimer(1000);
    }
}

void CsvFileOutput::update() {
    // TODO: check if keys contains a new key, and rewrite file if so

    // update file on interval
    if (outputFile.is_open() && writeTimer->check()) {
        if(rewriteRequired) {
            headers.clear();
            for (const auto& key : cache->getKeys()) {
                headers.push_back(key);
            }

            // write headers
            std::string headerLine;
            headerLine.append("timestamp");
            for (const auto& header : headers) {
                headerLine.append(fmt::format(",{}", header));
            }
            outputFile << fmt::format("{}\n", headerLine);
            rewriteRequired = false;
        }

        uint64_t timestamp = getCurrentMillis();

        // write line
        std::string line;
        line.append(fmt::format("{}", timestamp));
        for (const auto& header : headers) {
            line.append(fmt::format(",{}", cache->get(header)->value));
        }
        outputFile << fmt::format("{}\n", line);
    }

    if (flushTimer) {
        outputFile.flush();
    }

    // update cache
    for (auto& inDataPoint : in) {
        cache->set(inDataPoint->key, inDataPoint);
    }
}

CsvFileOutput::~CsvFileOutput() = default;