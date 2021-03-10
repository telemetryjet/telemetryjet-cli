#include "csv_file_input.h"
#include "utility/csv/csv_parser.h"
#include <boost/algorithm/string.hpp>

CsvFileInputDataSource::CsvFileInputDataSource(const json& definition) : FileInputDataSource(definition) {
    parseCsvSpecificOptions(definition);
}

void CsvFileInputDataSource::open() {
    FileInputDataSource::open();

    // if headers have not been overridden
    if (headers.empty() && isFirstLineHeader) {
        if (inputFile.is_open()) {
            std::string line;
            getline(inputFile, line);
            headers = parseCsvLine(line, separator);
            SM::getLogger()->info(fmt::format("[{}] CSV header length: {}", id, headers.size()));
        } else {
            throw std::runtime_error(fmt::format("Input file {} is not open.", filename));
        }
    }

    for (int i = 0; i < headers.size(); i++) {
        const auto& header = headers[i];
        if(header == timestampColumnName) {
            timestampColumn = i;
            hasTimestamp = true;
        }
    }
}

void CsvFileInputDataSource::update() {
    if (inputFile.is_open()) {
        std::string line;
        if (getline(inputFile, line)) {
            std::vector<std::string> values = parseCsvLine(line, separator);

            if (values.size() != headers.size()) {
                SM::getLogger()->warning(fmt::format(
                    "CSV file {}, line {}: Expected {} cells but found {}. Ignoring line.",
                    filename,
                    lineCount,
                    headers.size(),
                    values.size()));
            } else {
                uint64_t timestamp = hasTimestamp ? std::stoull(values[timestampColumn]) : 0;
                // TODO: convert to absolute timestamp in proper units

                // construct data points and add to the output queue
                for (int i = 0; i < headers.size(); i++) {
                    if (i == timestampColumn) {
                        continue;
                    }

                    write(createDataPointFromString(headers[i], timestamp, values[i]));
                    cellCount++;
                }
            }
            lineCount++;
        } else {
            SM::getLogger()->info(fmt::format("[{}] Finished reading {} lines, {} cells from {}", id, lineCount, cellCount, filename));
            state = INACTIVE;
        }
    } else {
        throw std::runtime_error(fmt::format("Input file {} is not open.", filename));
    }
}

void CsvFileInputDataSource::parseCsvSpecificOptions(const json& definition) {
    if (options.contains(FIRST_LINE_IS_HEADER_KEY)) {
        if (!options[FIRST_LINE_IS_HEADER_KEY].is_boolean()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects boolean for {}", id, type, FIRST_LINE_IS_HEADER_KEY));
        }
        isFirstLineHeader = options[FIRST_LINE_IS_HEADER_KEY];
    }

    if (options.contains(SEPARATOR_KEY)) {
        if (!options[SEPARATOR_KEY].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, SEPARATOR_KEY));
        }
        // TODO: validate sanitized separator
        separator = options[SEPARATOR_KEY];
    }

    if (options.contains(TIMESTAMP_COLUMN_NAME_KEY)) {
        if (!options[TIMESTAMP_COLUMN_NAME_KEY].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, TIMESTAMP_COLUMN_NAME_KEY));
        }
        timestampColumnName = options[TIMESTAMP_COLUMN_NAME_KEY];
    }

    if (options.contains(HEADERS_KEY)) {
        if (!options[HEADERS_KEY].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, HEADERS_KEY));
        }
        const std::string csvHeaderString = options[HEADERS_KEY];
        headers = parseCsvLine(csvHeaderString, separator);
    }

    if (options.contains(TIMESTAMP_TYPE_KEY)) {
        bool isValid = false;
        if (options[TIMESTAMP_TYPE_KEY].is_string()) {
            TimestampType timestampType = convertTimestampType(options[TIMESTAMP_TYPE_KEY]);
            if (timestampType == TimestampType::RELATIVE) {
                hasRelativeTimestamp = true;
            }
            isValid = (timestampType != TimestampType::UNKNOWN);
        }

        if (!isValid) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {} with value 'absolute' or 'relative'.", id, type, TIMESTAMP_TYPE_KEY));
        }
    }

    if (hasRelativeTimestamp) {
        if (options.contains(START_TIMESTAMP_KEY)) {
            if (!options[START_TIMESTAMP_KEY].is_number_integer()) {
                throw std::runtime_error(fmt::format("[{}] data source type '{}' expects integer for {}", id, type, START_TIMESTAMP_KEY));
            }
            startTimestamp = options[START_TIMESTAMP_KEY];
        }
    }

    if (options.contains(TIMESTAMP_UNITS_KEY)) {
        bool isValid = false;
        if (options[TIMESTAMP_UNITS_KEY].is_string()) {
            timestampUnits = convertTimestampUnits(options[TIMESTAMP_UNITS_KEY]);
            isValid = (timestampUnits != TimestampUnits::UNKNOWN);
        }

        if (!isValid) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {} with value 'seconds', 'milliseconds' or 'microseconds'.", id, type, TIMESTAMP_UNITS_KEY));
        }
    }
}
