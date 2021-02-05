#include "csv_file_input.h"
#include "utility/csv/csv_parser.h"

void CsvFileInputDataSource::open() {
    FileInputDataSource::open();

    // read in headers
    if (inputFile.is_open()) {
        std::string line;
        getline(inputFile, line);
        headers = parseCsvLine(line);
        hasTimestamp = (headers[0] == "timestamp");
    } else {
        throw std::runtime_error(fmt::format("Input file {} is not open.", filename));
    }
}

void CsvFileInputDataSource::update() {
    if (inputFile.is_open()) {
        std::string line;
        if (getline(inputFile, line)) {
            std::vector<std::string> values = parseCsvLine(line);

            if (values.size() != headers.size()) {
                SM::getLogger()->warning(fmt::format(
                    "CSV file {}, line {}: Expected {} cells but found {}. Ignoring line.",
                    filename,
                    lineCount,
                    headers.size(),
                    values.size()));
            } else {
                uint64_t timestamp = hasTimestamp ? std::stoull(values[0]) : 0;
                int startIdx = hasTimestamp ? 1 : 0;

                // construct data points and add to the output queue
                for (int i = startIdx; i < headers.size(); i++) {
                    write(createDataPointFromString(headers[i], timestamp, values[i]));
                }
            }
            lineCount++;
        } else {
            SM::getLogger()->info(fmt::format("[{}] Finished reading from {}", id, filename));
            state = INACTIVE;
        }
    } else {
        throw std::runtime_error(fmt::format("Input file {} is not open.", filename));
    }
}