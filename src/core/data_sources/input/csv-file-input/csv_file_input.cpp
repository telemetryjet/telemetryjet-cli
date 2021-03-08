#include "csv_file_input.h"
#include "utility/csv/csv_parser.h"
#include <boost/algorithm/string.hpp>

CsvFileInputDataSource::CsvFileInputDataSource(const json& definition) : FileInputDataSource(definition) {
    parseCsvSpecificOptions(definition);
}

void CsvFileInputDataSource::open() {
    FileInputDataSource::open();

    // read in headers
    if (inputFile.is_open()) {
        std::string line;
        getline(inputFile, line);
        headers = parseCsvLine(line);
        hasTimestamp = (headers[0] == "timestamp");
        SM::getLogger()->info(fmt::format("[{}] CSV header length: {}", id, headers.size()));
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
    const std::string firstLineIsHeaderKey = "firstLineIsHeader";
    if (options.contains(firstLineIsHeaderKey)) {
        if (!options[firstLineIsHeaderKey].is_boolean()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects boolean for {}", id, type, firstLineIsHeaderKey));
        }
        firstLineIsHeader = options[firstLineIsHeaderKey];
    }

    const std::string separatorKey = "separator";
    if (options.contains(separatorKey)) {
        if (!options[separatorKey].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, separatorKey));
        }
        separator = options[separatorKey];
    }

    const std::string timestampColumnNameKey = "timestampColumnName";
    if (options.contains(timestampColumnNameKey)) {
        if (!options[timestampColumnNameKey].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, timestampColumnNameKey));
        }
        timestampColumnName = options[timestampColumnNameKey];
    }

    const std::string headerStringKey = "headers";
    if (options.contains(headerStringKey)) {
        if (!options[headerStringKey].is_string()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {}", id, type, headerStringKey));
        }
        const std::string csvHeaderString = options[headerStringKey];
    }

    const std::string timestampTypeKey = "timestampType";
    if (options.contains(timestampTypeKey)) {
        bool isValid = false;
        if (options[timestampTypeKey].is_string()) {
            const std::string timestampType = boost::algorithm::to_lower_copy(static_cast<std::string>(options[timestampTypeKey]));
            // probably want to save these as constants somewhere instead of comparing strings
            hasRelativeTimestamp = (timestampType == "relative");
            isValid = (timestampType == "absolute" || timestampType == "relative");
        }

        if (!isValid) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {} with value 'absolute' or 'relative'.", id, type, timestampTypeKey));
        }
    }

    if (hasRelativeTimestamp) {
        const std::string startTimestampKey = "startTimestamp";
        if (options.contains(startTimestampKey)) {
            if (!options[startTimestampKey].is_number_integer()) {
                throw std::runtime_error(fmt::format("[{}] data source type '{}' expects integer for {}", id, type, startTimestampKey));
            }
            startTimestamp = options[startTimestampKey];
        }
    }

    const std::string timestampUnitsKey = "timestampUnits";
    if (options.contains(timestampUnitsKey)) {
        bool isValid = false;
        if (options[timestampUnitsKey].is_string()) {
            timestampUnits = boost::algorithm::to_lower_copy(static_cast<std::string>(options[timestampUnitsKey]));
            // probably want to save these as constants somewhere instead of comparing strings
            isValid = (timestampUnits == "seconds" || timestampUnits == "milliseconds" || timestampUnits == "microseconds");
        }

        if (!isValid) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' expects string for {} with value 'seconds', 'milliseconds' or 'microseconds'.", id, type, timestampUnitsKey));
        }
    }
}

