#include "csv_file_output.h"
#include "utility/path_utils.h"
#include "utility/csv/csv_parser.h"
#include <string>

namespace fs = boost::filesystem;

CsvFileOutputDataSource::CsvFileOutputDataSource(const json& definition): FileOutputDataSource(definition) {
    cache = new DataPointCache();
    rewriteRequired = true;

    if (options.contains("write_interval_ms")) {
        if (!options["write_interval_ms"].is_number_integer()) {
            throw std::runtime_error(fmt::format(
                "{} data source '{}' requires option 'write_interval_ms' of type Integer.",
                type,
                id));
        }
        writeInterval = options["write_interval_ms"];
        writeTimer = new SimpleTimer(writeInterval);
    } else {
        writeTimer = new SimpleTimer(0);
    }
}

void CsvFileOutputDataSource::open() {
    FileOutputDataSource::open();
    // if in append mode, read in the existing CSV headers
    if (modeString == "append") {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            getline(file, line);
            std::vector<std::string> parsedHeaders = parseCsvLine(line);

            // TODO: remove this requirement after fixing logic in update method
            if (parsedHeaders[0] != "timestamp")
            {
                std::string errorMsg = fmt::format("Parsing error in file {}. Expected first column of CSV file opened in append mode to be 'timestamp'.", filename);
                SM::getLogger()->error(errorMsg);
                throw std::runtime_error(errorMsg);
            }

            for (int i = 1; i < parsedHeaders.size(); i++)
            {
                const std::string& header = parsedHeaders[i];
                headerSet.insert(header);
                headers.push_back(header);
            }

            file.close();
        } else {
            throw std::runtime_error(fmt::format("Cannot read file at {}", filename));
        }
        rewriteRequired = false;
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
            const auto dp = cache->get(header);
            std::string val;
            if (dp != nullptr) {
                val = dp->type == DataPointType::STRING ? sanitizeString(dp->toString())
                                                        : dp->toString();
            }
            line.append(fmt::format(",{}", val));
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
        throw std::runtime_error(
            fmt::format("Failed to open temporary file {}.", tempPath.filename().string()));
    }

    // open input stream of current file
    std::ifstream inputFile(filename);

    // write updated headers to file
    std::string headerLine;
    headerLine.append(sanitizeString("timestamp"));
    for (const auto& header : headers) {
        headerLine.append(fmt::format(",{}", sanitizeString(header)));
    }
    tempFile << fmt::format("{}\n", headerLine);

    // write updated data to temporary file
    if (inputFile.is_open()) {
        std::string line;
        getline(inputFile, line);  // ignore headers

        std::regex newlines_re("\n+");
        while (getline(inputFile, line)) {
            tempFile << fmt::format("{}{}\n",
                                    std::regex_replace(line, newlines_re, ""),
                                    std::string(newHeaderCount, ','));
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

std::string CsvFileOutputDataSource::sanitizeString(const std::string& input) {
    std::string sanitized;
    std::regex newline_re("\n");
    std::regex quotes_re("\"");

    sanitized = std::regex_replace(input, newline_re, "\\n");
    sanitized = std::regex_replace(sanitized, quotes_re, "\"\"");
    return fmt::format("\"{}\"", sanitized);
}