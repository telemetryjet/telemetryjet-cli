#include "csv_file_output.h"
#include <string>

CsvFileOutputDataSource::CsvFileOutputDataSource(const std::string& id, const json& options)
    : FileOutputDataSource(id, "csv-file-output", options) {
    cache = new DataPointCache();
    rewriteRequired = true;

    if (options.contains("write_interval_ms")) {
        std::string interval;
        try {
            interval = options["write_interval_ms"];
            writeTimer = new SimpleTimer(std::stoi(interval));
        } catch (std::exception& e) {
            throw std::runtime_error(
                fmt::format("Error creating timer with parsed write_interval_ms '{}'. {}",
                            interval,
                            e.what()));
        }
    } else {
        writeTimer = new SimpleTimer(1000);
    }
}

void CsvFileOutputDataSource::update() {
    // update file on interval
    if (outputFile.is_open() && writeTimer->check()) {
        if (rewriteRequired) {
            rewrite();
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
        // if cache contains a header we haven't seen yet
        if (headerSet.insert(inDataPoint->key).second) {
            headers.push_back(inDataPoint->key);
            newHeaderCount++;
            rewriteRequired = true;
        }
    }
}

void CsvFileOutputDataSource::rewrite() {
    // close existing output file
    outputFile.close();

    std::ifstream inputFile(filename);

    // append commas for new headers to existing lines
    std::string updatedData;
    if (inputFile.is_open()) {
        std::string line;
        getline(inputFile, line);  // ignore headers

        std::regex newlines_re("\n+");
        while (getline(inputFile, line)) {
            updatedData.append(fmt::format("{}{}\n",
                                           std::regex_replace(line, newlines_re, ""),
                                           std::string(newHeaderCount, ',')));
        }
        inputFile.close();
    } else {
        throw std::runtime_error(fmt::format("Cannot read file at {}", filename));
    }

    // open output file in overwrite mode
    outputFile.open(filename, std::ios::trunc);

    // write updated data to file
    std::string headerLine;
    headerLine.append("timestamp");
    for (const auto& header : headers) {
        headerLine.append(fmt::format(",{}", header));
    }
    outputFile << fmt::format("{}\n{}", headerLine, updatedData);
    outputFile.flush();

    // close overwrite stream and reopen
    outputFile.close();
    outputFile.open(filename, mode);

    newHeaderCount = 0;
    rewriteRequired = false;
}

CsvFileOutputDataSource::~CsvFileOutputDataSource() = default;
