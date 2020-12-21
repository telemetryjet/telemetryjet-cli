#include "csv_file_output.h"
#include "utility/path_utils.h"
#include <string>

namespace fs = boost::filesystem;

CsvFileOutputDataSource::CsvFileOutputDataSource(const std::string& id, const json& options)
    : FileOutputDataSource(id, "csv-file-output", options) {
    cache = new DataPointCache();
    rewriteRequired = true;

    if (options.contains("write_interval_ms")) {
        if (!options["write_interval_ms"].is_number_integer()) {
            throw std::runtime_error(fmt::format("{} data source '{}' requires option 'write_interval_ms' of type Integer.", type, id));
        }
        writeInterval = options["write_interval_ms"];
        writeTimer = new SimpleTimer(writeInterval);
    } else {
        writeTimer = new SimpleTimer(0);
    }
}

void CsvFileOutputDataSource::update() {
    // update cache
    for (auto& inDataPoint : in) {
        cache->set(inDataPoint->key, inDataPoint);
        // if cache contains a header we haven't seen yet
        if (headerSet.insert(inDataPoint->key).second) {
            headers.push_back(inDataPoint->key);
            newHeaderCount++;
            rewriteRequired = true;
        }
        // Update last seen timestamp, which we will use when writing the row data if not in "write interval" mode
        if (lastTimestamp < inDataPoint->timestamp) {
            lastTimestamp = inDataPoint->timestamp;
        }
    }

    // If write interval has a number, do periodic writing
    // If write interval is 0, write whenever a new data point comes in
    bool shouldWriteRow;
    uint64_t timestamp = getCurrentMillis();
    if (writeInterval > 0) {
        shouldWriteRow = writeTimer->check();
    } else {
        shouldWriteRow = in.size() > 0;
        timestamp = lastTimestamp;
    }

    // update file on interval
    if (outputFile.is_open() && shouldWriteRow) {
        if (rewriteRequired) {
            rewrite();
        }


        // write line
        std::string line;
        line.append(fmt::format("{}", timestamp));
        for (const auto& header : headers) {
            line.append(fmt::format(",{}", cache->get(header)->toString()));
        }
        outputFile << fmt::format("{}\n", line);
    }

    if (flushTimer) {
        outputFile.flush();
    }
}

void CsvFileOutputDataSource::rewrite() {
    // close existing output file
    outputFile.close();

    // create temporary output file
    fs::path tempPath = fs::unique_path();
    std::ofstream tempFile = std::ofstream();
    tempFile.open(tempPath.string(), std::ios::trunc);

    if (!tempFile.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open temporary file {}.", tempPath.filename().string()));
    }

    // open input stream of current file
    std::ifstream inputFile(filename);

    // write updated headers to file
    std::string headerLine;
    headerLine.append("timestamp");
    for (const auto& header : headers) {
        headerLine.append(fmt::format(",{}", header));
    }
    tempFile << fmt::format("{}\n", headerLine);

    // write updated data to temporary file
    if (inputFile.is_open()) {
        std::string line;
        getline(inputFile, line);  // ignore headers

        std::regex newlines_re("\n+");
        while (getline(inputFile, line)) {
            tempFile << fmt::format("{}{}\n",std::regex_replace(line, newlines_re, ""),std::string(newHeaderCount, ','));
        }
        inputFile.close();
        tempFile.flush();
        tempFile.close();
    } else {
        throw std::runtime_error(fmt::format("Cannot read file at {}", filename));
    }

    // replace output file with temporary file
    fs::remove(filename);
    fs::rename(tempPath, filename);

    // reopen output file
    outputFile.open(filename, std::ios::app);

    newHeaderCount = 0;
    rewriteRequired = false;
}