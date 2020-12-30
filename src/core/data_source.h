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

using json = nlohmann::json;

class Network;

/**
 * Data Source State
 * Uninitialized -- open() not yet called successfully.
 * Active -- Data source is running.
 * Done -- Data source is running, but is done processing data.
 * Closed -- close() has been called and data source is inactive.
 */
typedef enum { UNINITIALIZED, ACTIVE, ACTIVE_OUTPUT_ONLY, INACTIVE, CLOSED } DataSourceState;

class DataSource : public std::enable_shared_from_this<DataSource> {
private:
    SQLite::Database* sqliteCache;
    std::mutex sqliteCacheMutex;

    void transferInCachedDataPoints();

protected:
    static std::mutex dependencyMutex;
    static std::map<std::string, int> dependencyMap;

public:
    std::shared_ptr<DataSource> getptr();
    const std::string id;
    const std::string type;
    bool online;
    json options;
    std::mutex inMutex;
    std::mutex outMutex;
    std::mutex initializationMutex;
    std::deque<std::shared_ptr<DataPoint>> _inQueue;
    std::deque<std::shared_ptr<DataPoint>> in;
    std::deque<std::shared_ptr<DataPoint>> out;
    Network* parent;

    std::atomic<DataSourceState> state = UNINITIALIZED;
    std::atomic<uint64_t> lastUpdated;
    std::atomic<bool> error = false;

    DataSource(const json& definition)
        : id(definition["id"])
        , type(definition["type"])
        , online(true) {
        lastUpdated = getCurrentMillis();
        if (definition.count("options") > 0) {
            options = definition["options"];
        }

        // init sqlite cache
        const std::lock_guard<std::mutex> lock(sqliteCacheMutex);
        std::string databasePath = fmt::format("file:./{}_cache.db", id);
        sqliteCache
            = new SQLite::Database(databasePath,
                                   SQLite::OPEN_URI | SQLite::OPEN_READWRITE
                                       | SQLite::OPEN_CREATE);  // NOLINT(hicpp-signed-bitwise)
        sqliteCache->exec(
            "create table if not exists data (id integer primary key, key text not null, timestamp "
            "text not null, data_type integer not null, value text not null)");
    };
    virtual void open();
    virtual void close();
    virtual void update();
    virtual void checkOnline();

    virtual void write(std::shared_ptr<DataPoint> dataPoint);
    virtual void writeImmediate(std::shared_ptr<DataPoint> dataPoint);

    void transferInDataPoints() {
        inMutex.lock();

        // if data source is online, transfer in any data points that are in the cache
        if (online) {
            transferInCachedDataPoints();
        }

        while (_inQueue.size() > 0) {
            in.push_back(_inQueue.front());
            _inQueue.pop_front();
        }
        inMutex.unlock();
    }

    void cacheIncomingDataPoints();

    // Flush list of output data points to be sent to the other data sources
    void flushDataPoints();

    // Assert that we are the only data source trying to access a given resource
    // Should use this check before any access to a resource that can't be shared
    // (Serial port, file, etc.)
    // This can be checked in the data source configuration, or anywhere else.
    void assertDependency(std::string key, std::string value, std::string errorMessage);
};

#endif
