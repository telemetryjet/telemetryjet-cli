//
// Created by chris on 12/3/2020.
//

#include <core/data_sources/test/test_data_source.h>
#include <core/data_sources/console/console_data_source.h>
#include <core/data_sources/system_stats/system_stats_data_source.h>
#include <core/data_sources/key_value_file/key_value_file.h>
#include "network.h"

Network::Network(const json& definitions) {
    // Construct array of data sources from definitions.
    for (auto& dataSourceDefinition : definitions) {
        if (dataSourceDefinition["type"] == "test") {
            json optionsNode = nullptr;
            if (dataSourceDefinition.contains("options")) {
                optionsNode = dataSourceDefinition["options"];
            }
            dataSources.push_back(std::make_shared<TestDataSource>(dataSourceDefinition["id"], optionsNode));
        }
        if (dataSourceDefinition["type"] == "console") {
            json optionsNode = nullptr;
            if (dataSourceDefinition.contains("options")) {
                optionsNode = dataSourceDefinition["options"];
            }
            dataSources.push_back(std::make_shared<ConsoleDataSource>(dataSourceDefinition["id"], optionsNode));
        }
        if (dataSourceDefinition["type"] == "system_stats") {
            dataSources.push_back(std::make_shared<SystemStatsDataSource>(dataSourceDefinition["id"]));
        }
        if (dataSourceDefinition["type"] == "key-value-file") {
            json optionsNode = nullptr;
            if (dataSourceDefinition.contains("options")) {
                optionsNode = dataSourceDefinition["options"];
            }
            dataSources.push_back(std::make_shared<KeyValueFile>(dataSourceDefinition["id"], optionsNode));
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

bool Network::isOpen() {
    for (auto& dataSource : dataSources) {
        if (dataSource->isOpen()) {
            return true;
        }
    }
    return false;
}
