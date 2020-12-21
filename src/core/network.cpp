#include <core/data_sources/bidirectional/key-value-stream/key_value_stream.h>
#include <core/data_sources/input/test-input/test_input.h>
#include <core/data_sources/output/console-output/console_output.h>
#include <core/data_sources/output/csv-file-output/csv_file_output.h>
#include <core/data_sources/output/key-value-file-output/key_value_file_output.h>
#include <core/data_sources/input/key-value-file-input/key_value_file_input.h>
#include <core/data_sources/input/nmea-0183-file-input/nmea_0183_file_input.h>
#include <core/data_sources/input/nmea-0183-stream/nmea_0183_stream.h>
#include <core/data_sources/input/system-stats/system_stats.h>
#include <core/data_sources/input/joystick/joystick.h>
#include "network.h"

Network::Network(const json& definitions) {
    // Construct array of data sources from definitions.
    for (auto& dataSourceDefinition : definitions) {
        json optionsNode = nullptr;
        if (dataSourceDefinition.contains("options")) {
            optionsNode = dataSourceDefinition["options"];
        }

        std::string id = dataSourceDefinition["id"];
        std::string type = dataSourceDefinition["type"];
        if (type == "key-value-stream") {
            dataSources.push_back(std::make_shared<KeyValueStream>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "test-input") {
            dataSources.push_back(std::make_shared<TestInputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "console-output") {
            dataSources.push_back(std::make_shared<ConsoleOutputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "csv-file-output") {
            dataSources.push_back(std::make_shared<CsvFileOutputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "key-value-file-output") {
            dataSources.push_back(std::make_shared<KeyValueFileOutputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "key-value-file-input") {
            dataSources.push_back(std::make_shared<KeyValueFileInputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "nmea-0183-file-input") {
            dataSources.push_back(std::make_shared<NMEA0183FileInputDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "nmea-0183-stream") {
            dataSources.push_back(std::make_shared<NMEA0183StreamDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "system-stats") {
            dataSources.push_back(std::make_shared<SystemStatsDataSource>(dataSourceDefinition["id"], optionsNode));
        } else if (type == "joystick") {
            dataSources.push_back(std::make_shared<JoystickDataSource>(dataSourceDefinition["id"], optionsNode));
        } else {
            throw std::runtime_error(fmt::format("Data source '{}' has unknown type {}.", id, type));
        }
    }
}

Network::~Network() {
}

void Network::start() {
    for (auto& dataSource : dataSources) {
        dataSource->open();
    }
}

void Network::update() {
    // Update all data sources
    // Data sources will take in values from the last iteration,
    // and output values for the next iteration.
    for (auto& dataSource : dataSources) {
        dataSource->update();
        // Clear inputs from this data source.
        // Some data sources never handle inputs, so if they ignore the data, it gets cleared.
        dataSource->in.clear();
    }
    // Any data sources that produced new values, add them to the other data source inputs
    // The other data sources will recieve this data on the next iteration
    for (auto& dataSourceOut : dataSources) {
        if (!dataSourceOut->out.empty()) {
            //SM::getLogger()->info(fmt::format("Data source {} has {} output values", dataSourceOut->id, dataSourceOut->out.size()));
            for (auto& dataSourceIn : dataSources) {
                if (dataSourceIn != dataSourceOut) {
                    //SM::getLogger()->info(fmt::format("Writing {} output values from {} -> {}", dataSourceOut->out.size(), dataSourceOut->id, dataSourceIn->id));
                    for (auto& dataPoint : dataSourceOut->out) {
                        dataSourceIn->in.push_back(dataPoint);
                    }
                }
            }
            // Clear the outputs after copying them to the other files
            dataSourceOut->out.clear();
        }
    }
}

void Network::stop() {
    for (auto& dataSource : dataSources) {
        dataSource->close();
    }
}

bool Network::checkDone() {
    // Exit if all the devices have a 'done' flag
    bool allDone = true;
    for (auto& dataSource : dataSources) {
        if (!dataSource->checkDone()) {
            allDone = false;
        }
    }
    return allDone;
}

bool Network::checkExitOnError() {
    // Exit if any of the devices has an error and we are in --exit mode
    for (auto& dataSource : dataSources) {
        if (dataSource->checkExitOnError()) {
            return true;
        }
    }
    return false;
}