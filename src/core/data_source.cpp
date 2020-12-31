#include "data_source.h"
#include "network.h"

std::mutex DataSource::dependencyMutex;
std::map<std::string, int> DataSource::dependencyMap;

void DataSource::assertDependency(std::string key, std::string value, std::string errorMessage) {
    std::unique_lock<std::mutex> lock(dependencyMutex);
    std::string combinedKey = fmt::format("{}__{}", key, value);
    if (dependencyMap.count(combinedKey) == 0) {
        dependencyMap[combinedKey] = 1;
    } else {
        throw std::runtime_error(errorMessage);
    }
}

void DataSource::open() {
    state = ACTIVE;
}

void DataSource::close() {
    state = CLOSED;
}

void DataSource::update() {
}

void DataSource::checkOnline() {
}

// Write to the queue, to be output to all other data sources on the next iteration
void DataSource::write(std::shared_ptr<DataPoint> dataPoint) {
    outMutex.lock();
    out.push_back(dataPoint);
    outMutex.unlock();
}

// Write to the queue immediately.
// Useful if the update() function is performing some type of blocking logic
void DataSource::writeImmediate(std::shared_ptr<DataPoint> dataPoint) {
    write(dataPoint);
    flushDataPoints();
}

void DataSource::flushDataPoints() {
    parent->propagateDataPoints(getptr());
}

std::shared_ptr<DataSource> DataSource::getptr() {
    return shared_from_this();
}

void DataSource::cacheIncomingDataPoints() {
    if (!in.empty()) {
        const std::lock_guard<std::mutex> lock(sqliteCacheMutex);

        for (auto& dp : in) {
            try {
                SQLite::Statement insertStatement(*sqliteCache,
                                                  "insert into data values (null,?,?,?,?)");
                insertStatement.bind(1, dp->key);
                insertStatement.bind(2, static_cast<long long>(dp->timestamp));
                insertStatement.bind(3, static_cast<int>(dp->type));
                insertStatement.bind(4, dp->toString());
                insertStatement.exec();
            } catch (std::exception& e) {
                SM::getLogger()->warning(fmt::format("[{}] Warning: Failed to cache data point to SQLite database: {}", e.what()));
            }
        }
    }
}

void DataSource::transferInCachedDataPoints() {
    // transfer data points to in queue
    const std::lock_guard<std::mutex> lock(sqliteCacheMutex);

    bool deleteCache = false;
    try {
        SQLite::Statement query(*sqliteCache, "select * from data order by timestamp asc");
        while (query.executeStep()) {
            deleteCache = true;
            std::string key = query.getColumn(1);
            uint64_t timestamp = query.getColumn(2).getInt64();
            std::string value = query.getColumn(4);
            in.push_back(createDataPointFromString(key, timestamp, value));
        }
    } catch (std::exception& e) {
        throw std::runtime_error(fmt::format("Error getting data from sqlite cache. {}", e.what()));
    }

    // delete data points from cache
    if (deleteCache) {
        try {
            SQLite::Statement deleteStatement(*sqliteCache, fmt::format("delete from data"));
            deleteStatement.exec();
        } catch (std::exception& e) {
            throw std::runtime_error(
                fmt::format("Error deleting all data from sqlite cache. {}", e.what()));
        }
    }
}