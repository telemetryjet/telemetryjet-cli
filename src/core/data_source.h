#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "data_point.h"
#include "fmt/format.h"
#include "services/service_manager.h"
#include "utility/time_utils.h"
#include <SQLiteCpp/Database.h>
#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <utility>
#include <vector>
#include "utility/path_utils.h"
#include "utility/timer/simple_timer.h"

using json = nlohmann::json;

class Network;

typedef enum { UNINITIALIZED, ACTIVE, INACTIVE, CLOSED } DataSourceState;

class DataSource : public std::enable_shared_from_this<DataSource> {
private:

protected:
    // Dependency tracking (Files, ports, etc) between data sources
    static std::mutex dependencyMutex;
    static std::map<std::string, int> dependencyMap;

public:
    // Data source metadata
    json options;
    Network* parent;
    const std::string id;
    const std::string type;
    std::atomic<bool> isOnline = true;
    std::atomic<bool> hasError = false;

    // Mode parameters and settings
    std::atomic<bool> inputEnabled = true;
    std::atomic<bool> outputEnabled = true;
    std::atomic<DataSourceState> state = UNINITIALIZED;
    std::atomic<uint64_t> lastUpdated;

    // Input and output queues; thread synchronization
    std::shared_ptr<DataSource> getptr();
    std::mutex inMutex;
    std::mutex outMutex;
    std::mutex initializationMutex;
    std::deque<std::shared_ptr<DataPoint>> _inQueue;
    std::deque<std::shared_ptr<DataPoint>> in;
    std::deque<std::shared_ptr<DataPoint>> out;

    // Input and output filters
    std::set<std::string> inputFilter;
    std::set<std::string> outputFilter;
    bool hasInputFilter = false;
    bool hasOutputFilter = false;

    // Rate limiting
    // Rate limit imposes a limit on both 1) Number of data points per second 2) Number of data points per iteration
    std::atomic<int> rateLimit = INT_MAX;
    std::atomic<int> rateLimitCounter = 0;
    std::unique_ptr<SimpleTimer> rateLimitResetTimer;

    // SQLite data point cache
    std::atomic<bool> hasCache = false;
    std::atomic<int> cacheNumItems = 0;
    std::string cacheFilename;
    SQLite::Database* sqliteCache;
    std::mutex sqliteCacheMutex;

    DataSource(const json& definition)
        : id(definition["id"])
        , type(definition["type"]) {
        assertDependency("id", id, fmt::format("[{}] Multiple data sources cannot share the same ID: {}", id, id));

        lastUpdated = getCurrentMillis();
        if (definition.count("options") > 0) {
            options = definition["options"];
        }

        if (definition.contains("input") && definition["input"].is_boolean()) {
            inputEnabled = definition["input"];
        }
        if (definition.contains("output") && definition["output"].is_boolean()) {
            outputEnabled = definition["output"];
        }
        if (options.contains("rateLimit") && options["rateLimit"].is_number_integer()) {
            rateLimit = options["rateLimit"];
        }

        if (options.contains("inputFilter")) {
            if (!options["inputFilter"].is_array()) {
                throw std::runtime_error(fmt::format("[{}] option 'inputFilter' must be array!", id));
            } else {
                hasInputFilter = true;
                for (auto& filterItem : options["inputFilter"]) {
                    if (!filterItem.is_string()) {
                        throw std::runtime_error(fmt::format("[{}] All items in 'inputFilter' array must be of type String!", id));
                    }
                    inputFilter.insert(filterItem.get<std::string>());
                }
            }
        }
        if (options.contains("outputFilter")) {
            if (!options["outputFilter"].is_array()) {
                throw std::runtime_error(fmt::format("[{}] option 'outputFilter' must be array!", id));
            } else {
                hasOutputFilter = true;
                for (auto& filterItem : options["outputFilter"]) {
                    if (!filterItem.is_string()) {
                        throw std::runtime_error(fmt::format("[{}] All items in 'outputFilter' array must be of type String!", id));
                    }
                    outputFilter.insert(filterItem.get<std::string>());
                }
            }
        }

        json cacheConfig;
        if (options.contains("cache")) {
            if (options["cache"].is_object()) {
                cacheConfig = options["cache"];
                if (cacheConfig.contains("enabled")) {
                    if (!cacheConfig["enabled"].is_boolean()) {
                        throw std::runtime_error(fmt::format("[{}] data source cache requires option 'enabled' of type Boolean", id, type));
                    }
                    if (cacheConfig["enabled"]) {
                        hasCache = true;
                        if (cacheConfig.contains("filename")) {
                            if (!cacheConfig["filename"].is_string()) {
                                throw std::runtime_error(
                                        fmt::format("[{}] data source cache requires option 'filename' of type String", id, type));
                            }
                            cacheFilename = resolveRelativePathHome(cacheConfig["filename"]);
                        } else {
                            cacheFilename = resolveRelativePathHome(fmt::format("{}_cache.db", id));
                        }
                        assertDependency("file", cacheFilename, fmt::format("[{}] Multiple data sources cannot share the same input/output filename: {}", id, cacheFilename));
                    }
                }
            } else {
                throw std::runtime_error(fmt::format("[{}] 'cache' configuration must be an Object", id, type));
            }
        }
        rateLimitResetTimer = std::make_unique<SimpleTimer>(1000);
    };
    virtual void open();
    virtual void close();
    virtual void update();
    virtual void checkOnline();

    virtual void write(std::shared_ptr<DataPoint> dataPoint);
    virtual void writeImmediate(std::shared_ptr<DataPoint> dataPoint);

    void startUpdate();
    void finishUpdate();

    void initializeCache(); // Initialize the cache
    void closeCache(); // Close the cache and cleanup resources
    void readCacheValues(int batchSize); // Read a batch from the cache file into the input queue if it's allowed
    void writeCacheValues(); // Write all values on the input queue into the cache file

    // Flush list of output data points to be sent to the other data sources
    void flushDataPoints();

    // Assert that we are the only data source trying to access a given resource
    // Should use this check before any access to a resource that can't be shared
    // (Serial port, file, etc.)
    // This can be checked in the data source configuration, or anywhere else.
    void assertDependency(std::string key, std::string value, std::string errorMessage);
};

#endif
