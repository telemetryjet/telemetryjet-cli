#include <core/data_sources/bidirectional/key-value-stream/key_value_stream.h>
#include <core/data_sources/input/test-input/test_input.h>
#include <core/data_sources/output/console-output/console_output.h>
#include <core/data_sources/input/csv-file-input/csv_file_input.h>
#include <core/data_sources/output/csv-file-output/csv_file_output.h>
#include <core/data_sources/output/key-value-file-output/key_value_file_output.h>
#include <core/data_sources/output/aws-kinesis-firehose/aws_kinesis_firehose.h>
#include <core/data_sources/input/key-value-file-input/key_value_file_input.h>
#include <core/data_sources/input/nmea-0183-file-input/nmea_0183_file_input.h>
#include <core/data_sources/input/nmea-0183-stream/nmea_0183_stream.h>
#include <core/data_sources/input/system-stats/system_stats.h>
#include <core/data_sources/input/joystick/joystick.h>
#include <core/data_sources/output/sqlite-output/sqlite_output.h>
#include <core/data_sources/input/console-input/console_input.h>
#include <core/data_sources/bidirectional/websocket/client/websocket_client.h>
#include <core/data_sources/bidirectional/websocket/server/websocket_server.h>
#include <core/data_sources/bidirectional/telemetryjet-server/telemetryjet_server.h>
#include <core/data_sources/bidirectional/telemetryjet-arduino-sdk/telemetryjet_arduino_sdk.h>
#include "network.h"
#include <boost/lexical_cast.hpp>

Network::Network(const json& definitions, bool errorMode): errorMode(errorMode) {
    // Construct array of data sources from definitions.
    for (auto& dataSourceDefinition : definitions) {
        std::string id = dataSourceDefinition["id"];
        std::string type = dataSourceDefinition["type"];
        if (type == "key-value-stream") {
            dataSources.push_back(std::make_shared<KeyValueStream>(dataSourceDefinition));
        } else if (type == "test-input") {
            dataSources.push_back(std::make_shared<TestInputDataSource>(dataSourceDefinition));
        } else if (type == "console-output") {
            dataSources.push_back(std::make_shared<ConsoleOutputDataSource>(dataSourceDefinition));
        } else if (type == "console-input") {
            dataSources.push_back(std::make_shared<ConsoleInputDataSource>(dataSourceDefinition));
        } else if (type == "csv-file-output") {
            dataSources.push_back(std::make_shared<CsvFileOutputDataSource>(dataSourceDefinition));
        } else if (type == "csv-file-input") {
            dataSources.push_back(std::make_shared<CsvFileInputDataSource>(dataSourceDefinition));
        } else if (type == "key-value-file-output") {
            dataSources.push_back(std::make_shared<KeyValueFileOutputDataSource>(dataSourceDefinition));
        } else if (type == "key-value-file-input") {
            dataSources.push_back(std::make_shared<KeyValueFileInputDataSource>(dataSourceDefinition));
        } else if (type == "nmea-0183-file-input") {
            dataSources.push_back(std::make_shared<NMEA0183FileInputDataSource>(dataSourceDefinition));
        } else if (type == "nmea-0183-stream") {
            dataSources.push_back(std::make_shared<NMEA0183StreamDataSource>(dataSourceDefinition));
        } else if (type == "system-stats") {
            dataSources.push_back(std::make_shared<SystemStatsDataSource>(dataSourceDefinition));
        } else if (type == "joystick") {
            dataSources.push_back(std::make_shared<JoystickDataSource>(dataSourceDefinition));
        } else if (type == "sql-table-output") {
            dataSources.push_back(std::make_shared<SqliteOutputDataSource>(dataSourceDefinition));
        } else if (type == "websocket-client") {
            dataSources.push_back(std::make_shared<WebsocketClientDataSource>(dataSourceDefinition));
        } else if (type == "websocket-server") {
            dataSources.push_back(std::make_shared<WebsocketServerDataSource>(dataSourceDefinition));
        } else if (type == "telemetryjet-server") {
            dataSources.push_back(std::make_shared<TelemetryJetServerDataSource>(dataSourceDefinition));
        } else if (type == "telemetryjet-arduino-sdk") {
            dataSources.push_back(std::make_shared<TelemetryJetArduinoSdkDataSource>(dataSourceDefinition));
        } else if (type == "aws-kinesis-firehose") {
            dataSources.push_back(std::make_shared<AwsKinesisFirehoseDataSource>(dataSourceDefinition));
        } else {
            throw std::runtime_error(fmt::format("[{}] Data source has unknown type {}.", id, type));
        }
        dataSources.back()->parent = this;
    }

    dataSourceCheckTimer = std::make_shared<SimpleTimer>(5000);
}

// Utility function that runs in a worker thread for each data source
// Handles the complete lifecycle of a data source:
// initialization, main loop, and cleanup.
void dataSourceThread(std::shared_ptr<DataSource> dataSource, bool errorMode) {
    while (dataSource->state == ACTIVE) {
        try {
            boost::this_thread::interruption_point();

            // Update the online status of the data source
            dataSource->checkOnline();

            // Update the data source
            dataSource->startUpdate();
            dataSource->update();
            dataSource->finishUpdate();

        } catch (boost::thread_interrupted& e) {
            dataSource->state = INACTIVE;
        } catch (std::exception &e) {
            SM::getLogger()->error(fmt::format("[{}] {}", dataSource->id, e.what()));
            dataSource->parent->error = true;
            dataSource->state = INACTIVE;
        }
    }

    dataSource->close();
}

void Network::start() {
    bool _errMode = errorMode;
    // Initialize data source and open a thread to execute updates
    for (auto& dataSource: dataSources) {
        dataSource->open();
        dataSource->initializationMutex.lock();
        if (dataSource->state == UNINITIALIZED) {
            throw std::runtime_error(fmt::format("[{}] Data source in uninitialized (invalid) state!", dataSource->id));
        }
        dataSourceWorkerThreads.push_back(std::make_shared<boost::thread>([dataSource, _errMode](){
            // Delay start of thread until all data sources have been initialized
            auto threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
            dataSource->initializationMutex.lock();
            SM::getLogger()->info(fmt::format("[{}] Started worker thread with ID {}", dataSource->id, threadId));
            dataSourceThread(dataSource, _errMode);
            dataSource->initializationMutex.unlock();
            SM::getLogger()->info(fmt::format("[{}] Finished worker thread with ID {}", dataSource->id, threadId));
        }));
    }
}

void Network::releaseDataSourceInitMutexes() {
    // Unlock all the data sources, releasing them to start handling data at the same time
    for (auto& dataSource: dataSources) {
        dataSource->initializationMutex.unlock();
    }
}

void Network::stop() {
    // Set interrupt flag on the network.
    // This stops propagation of any new data points between sources.
    // Some data sources may not encounter the interrupt signal for some time, so this safeguards against that.
    interrupted = true;

    // Send interrupt signal, indicating the threads should stop.
    for (auto& dataSourceWorkerThread: dataSourceWorkerThreads) {
        dataSourceWorkerThread->interrupt();
    }

    // Join all threads. This will block until all threads have stopped.
    for (auto& dataSourceWorkerThread: dataSourceWorkerThreads) {
        dataSourceWorkerThread->join();
    }
}

bool Network::isInitialized() {
    // Check that every data source was successfully initialized
    for (auto& dataSource: dataSources) {
        if (dataSource->state == UNINITIALIZED) {
            return false;
        }
    }
    return true;
}

bool Network::isDone() {
    // Check the state of all data sources
    // If all data sources are done, we can exit
    bool allDone = true;
    for (auto& dataSource: dataSources) {
        if (dataSource->inputEnabled && dataSource->state == ACTIVE) {
            allDone = false;
        }
        // Also give a chance for the output-only data sources to output their full queues
        // Prevents race conditions on exit; we want the output to flush completely at least once
        // to get the last data points from an input file.
        if (dataSource->outputEnabled) {
            if (!dataSource->_inQueue.empty() || !dataSource->in.empty()) {
                allDone = false;
            }
            if (dataSource->hasCache && dataSource->cacheNumItems > 0) {
                allDone = false;
            }
        }
    }
    return allDone;
}

void Network::checkDataSources() {
    if (dataSourceCheckTimer->check()) {
        for (auto& dataSource: dataSources) {
            if (dataSource->state == ACTIVE) {
                // Check for errors
                if (errorMode) {
                    if (dataSource->hasError) {
                        SM::getLogger()->error(fmt::format("[{}] Error in data source, exiting...", dataSource->id));
                        error = true;
                    }
                }

                // Check for long-running updates
                uint64_t dataSourceLastUpdatedDelta = getCurrentMillis() - dataSource->lastUpdated;
                if (dataSourceLastUpdatedDelta > 5000) {
                    SM::getLogger()->warning(fmt::format("[{}] Warning: Data source stuck in update ({} ms)", dataSource->id, dataSourceLastUpdatedDelta));
                }
            }
        }
    }
}

// Write from a data source's output queue into all the other data sources
void Network::propagateDataPoints(std::shared_ptr<DataSource> dataSourceOut) {
    dataSourceOut->outMutex.lock();
    if (!dataSourceOut->out.empty()) {
        // Add prefix ID to all data points
        //for (auto& dataPoint : dataSourceOut->out) {
        //    dataPoint->key = fmt::format("{}.{}", dataSourceOut->id, dataPoint->key);
        //}

        if (!interrupted) {
            for (auto& dataSourceIn : dataSources) {
                if (dataSourceIn != dataSourceOut) {
                    dataSourceIn->inMutex.lock();
                    for (auto& dataPoint : dataSourceOut->out) {
                        dataSourceIn->_inQueue.push_back(dataPoint);
                    }
                    dataSourceIn->inMutex.unlock();
                }
            }
        }
        // Clear the outputs after copying them to the other data sources
        dataSourceOut->out.clear();
    }
    dataSourceOut->outMutex.unlock();
}