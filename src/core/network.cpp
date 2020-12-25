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
        } else if (type == "csv-file-output") {
            dataSources.push_back(std::make_shared<CsvFileOutputDataSource>(dataSourceDefinition));
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
        } else {
            throw std::runtime_error(fmt::format("Data source '{}' has unknown type {}.", id, type));
        }
        dataSources.back()->parent = this;
    }

    dataSourceCheckTimer = std::make_shared<SimpleTimer>(5000);
}

// Utility function that runs in a worker thread for each data source
// Handles the complete lifecycle of a data source:
// initialization, main loop, and cleanup.
void dataSourceThread(std::shared_ptr<DataSource> dataSource, bool errorMode) {
    while (dataSource->state == ACTIVE || dataSource->state == ACTIVE_OUTPUT_ONLY) {
        try {
            boost::this_thread::interruption_point();

            // Move any in data points into the internal queue.
            // This prevents locking if a data source has a long-running update call.
            dataSource->transferInDataPoints();

            // Update the data source
            dataSource->update();

            // Set a "last updated" time. The network uses this to detect situations where a data source
            // is stuck in a single update for a significant amount of time, and logs a warning.
            dataSource->lastUpdated = getCurrentMillis();

            // Clear inputs from this data source.
            // Some data sources never handle inputs, so if they ignore the data, it gets cleared.
            // This prevents input cache buildup and an eventual OOM error.
            dataSource->in.clear();

            // Flush any output data points into the other data sources
            dataSource->flushDataPoints();
        } catch (boost::thread_interrupted& e) {
            dataSource->state = INACTIVE;
        } catch (std::runtime_error &e) {
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
        try {
            dataSource->open();
            if (dataSource->state == UNINITIALIZED) {
                throw std::runtime_error("Data source in uninitialized (invalid) state!");
            }
            dataSourceWorkerThreads.push_back(std::make_shared<boost::thread>([dataSource, _errMode](){
                SM::getLogger()->info(fmt::format("[{}] Started thread.", dataSource->id));
                dataSourceThread(dataSource, _errMode);
                SM::getLogger()->info(fmt::format("[{}] Finished thread.", dataSource->id));
            }));
        } catch (std::runtime_error &e) {
            dataSource->state = CLOSED;
            dataSource->parent->error = true;
            return;
        }
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
        if (dataSource->state == ACTIVE) {
            allDone = false;
        }
    }
    return allDone;
}

void Network::checkDataSources() {
    if (dataSourceCheckTimer->check()) {
        for (auto& dataSource: dataSources) {
            // Check for errors
            if (errorMode) {
                if (dataSource->error) {
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

// Write from a data source's output queue into all the other data sources
// This MUST be called from within the same thread that the data source is running on
// The output queue is not guarded with a mutex
void Network::propagateDataPoints(std::shared_ptr<DataSource> dataSourceOut) {
    if (!dataSourceOut->out.empty()) {
        // Add prefix ID to all data points
        for (auto& dataPoint : dataSourceOut->out) {
            dataPoint->key = fmt::format("{}.{}", dataSourceOut->id, dataPoint->key);
        }

        if (!interrupted) {
            for (auto& dataSourceIn : dataSources) {
                if (dataSourceIn != dataSourceOut) {
                    for (auto& dataPoint : dataSourceOut->out) {
                        dataSourceIn->inMutex.lock();
                        dataSourceIn->_inQueue.push_back(dataPoint);
                        dataSourceIn->inMutex.unlock();
                    }
                }
            }
        }
        // Clear the outputs after copying them to the other data sources
        dataSourceOut->out.clear();
    }
}
