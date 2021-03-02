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
    if (hasCache) {
        initializeCache();
    }
    if (inputEnabled || outputEnabled) {
        state = ACTIVE;
    } else {
        state = INACTIVE;
    }
}

void DataSource::close() {
    if (hasCache) {
        closeCache();
    }
    state = CLOSED;
}

void DataSource::update() {
}

void DataSource::checkOnline() {
}

// Write to the queue, to be output to all other data sources on the next iteration
void DataSource::write(std::shared_ptr<DataPoint> dataPoint) {
    // Skip data point if a filter is enabled and there is no value
    if (hasInputFilter && inputFilter.count(dataPoint->key) == 0) {
        return;
    }

    if (inputEnabled) {
        outMutex.lock();
        out.push_back(dataPoint);
        outMutex.unlock();
    }
}

// Write to the queue immediately.
// Useful if the update() function is performing some type of blocking logic
void DataSource::writeImmediate(std::shared_ptr<DataPoint> dataPoint) {
    // Skip data point if a filter is enabled and there is no value
    if (hasInputFilter && inputFilter.count(dataPoint->key) == 0) {
        return;
    }

    if (inputEnabled) {
        write(dataPoint);
        flushDataPoints();
    }
}

void DataSource::flushDataPoints() {
    parent->propagateDataPoints(getptr());
}

std::shared_ptr<DataSource> DataSource::getptr() {
    return shared_from_this();
}

void DataSource::startUpdate() {
    if (outputEnabled) {
        // Check the rate limit counter
        // Rate limit is reset once a second
        if (rateLimitResetTimer->check()) {
            if (!hasCache && rateLimitCounter >= rateLimit) {
                SM::getLogger()->warning(fmt::format("[{}] Rate limited to {} data points/second, {} data points were dropped!", id, rateLimit, (rateLimitCounter - rateLimit)));
            }
            rateLimitCounter = 0;
        }

        // Transfer all data points from the shared input queue into the private input queue
        // Ignore data points if the data source hits a rate limit
        inMutex.lock();
        while (!_inQueue.empty()) {
            rateLimitCounter += 1;
            if (!hasCache && rateLimitCounter >= rateLimit) {
                break;
            }
            auto& inDp = _inQueue.front();

            // Skip data point if a filter is enabled and there is no value
            if (hasOutputFilter) {
                if (outputFilter.count(inDp->key) > 0) {
                    in.push_back(inDp);
                }
            } else {
                in.push_back(inDp);
            }

            _inQueue.pop_front();
        }
        _inQueue.clear();
        inMutex.unlock();
    } else {
        inMutex.lock();
        _inQueue.clear();
        inMutex.unlock();
    }

    // Read/write values to the cache
    if (outputEnabled && hasCache) {
        writeCacheValues();

        if (rateLimitCounter < rateLimit) {
            readCacheValues(std::min(100, std::max(0, (rateLimit - rateLimitCounter))));
        }
    }
}

void DataSource::finishUpdate() {
    if (in.size() > 0) {
        // Clear inputs from this data source.
        // Some data sources never handle inputs, so if they ignore the data, it gets cleared.
        // This prevents input cache buildup and an eventual OOM error.
        in.clear();
    }

    // Flush any output data points into the other data sources
    if (inputEnabled) {
        flushDataPoints();
    } else {
        outMutex.lock();
        out.clear();
        outMutex.unlock();
    }

    // Set a "last updated" time. The network uses this to detect situations where a data source
    // is stuck in a single update for a significant amount of time, and logs a warning.
    lastUpdated = getCurrentMillis();
}

void DataSource::initializeCache() {
    const std::lock_guard<std::mutex> lock(sqliteCacheMutex);
    std::string databasePath = cacheFilename;
    sqliteCache = new SQLite::Database(fmt::format("file:{}", cacheFilename),
                                    SQLite::OPEN_URI              // NOLINT(hicpp-signed-bitwise)
                                            | SQLite::OPEN_READWRITE    // NOLINT(hicpp-signed-bitwise)
                                            | SQLite::OPEN_CREATE);     // NOLINT(hicpp-signed-bitwise)
    sqliteCache->exec(
            "create table if not exists data (id integer primary key, key text not null, timestamp "
            "text not null, data_type integer not null, value text not null)");

    // Get count of number of items in the cache
    SQLite::Statement countQuery(*sqliteCache, "select count(*) from data");
    while (countQuery.executeStep()) {
        cacheNumItems = countQuery.getColumn(0).getInt64();
    }
    SM::getLogger()->info(fmt::format("[{}] Initialized data point cache.", id));
    SM::getLogger()->info(fmt::format("[{}] Data point cache has {} stored values.", id, cacheNumItems));
}

void DataSource::readCacheValues(int batchSize) {
    // Don't load if we have no items in the cache
    if (cacheNumItems <= 0) {
        return;
    }
    // Don't load if the data source is not online
    if (!isOnline) {
        return;
    }

    const std::lock_guard<std::mutex> lock(sqliteCacheMutex);
    try {
        SQLite::Statement query(*sqliteCache,fmt::format("select * from data order by timestamp asc limit {}", batchSize));
        std::vector<uint64_t> idList;
        while (query.executeStep()) {
            uint64_t id = query.getColumn(0).getInt64();
            std::string key = query.getColumn(1);
            uint64_t timestamp = query.getColumn(2).getInt64();
            uint64_t dataType = query.getColumn(3).getInt64();
            std::string value = query.getColumn(4);
            in.push_back(createDataPointFromString(key, timestamp, value));
            idList.push_back(id);
        }
        // Delete rows we just selected
        std::string idListString;
        for (int i = 0; i < idList.size(); i++) {
            if (i < idList.size() - 1) {
                idListString += fmt::format("{}, ", idList[i]);
            } else {
                idListString += fmt::format("{}", idList[i]);
            }
        }
        SQLite::Statement deleteStatement(*sqliteCache, fmt::format("delete from data where id in ({})", idListString));
        deleteStatement.exec();
    } catch (std::exception& e) {
        throw std::runtime_error(fmt::format("[{}] Error getting data from sqlite cache. {}", id, e.what()));
    }
    rateLimitCounter += in.size();
    cacheNumItems -= in.size();
    SM::getLogger()->info(fmt::format("[{}] Read {} data points from cache. Cache size: {} items", id, in.size(), cacheNumItems));
}

// Write all values on the input queue into the cache
void DataSource::writeCacheValues() {
    while (!in.empty()) {
        int numDataPointsWritten = 0;
        int batchSize = std::min((int)100, (int)in.size());
        std::deque<std::shared_ptr<DataPoint>> tempIn;
        for (int idx = 0; idx < batchSize; idx++) {
            tempIn.push_back(in.front());
            in.pop_front();
        }

        const std::lock_guard<std::mutex> lockSqlite(sqliteCacheMutex);
        std::string preparedStatement = "insert into data values ";
        int idx = 0;
        for (auto& dp : tempIn) {
            if (idx < tempIn.size() - 1) {
                preparedStatement += "(null,?,?,?,?),";
            } else {
                preparedStatement += "(null,?,?,?,?)";
            }
            idx++;
            numDataPointsWritten++;
        }
        SQLite::Statement insertStatement(*sqliteCache, preparedStatement);
        idx = 0;
        for (auto& dp : tempIn) {
            insertStatement.bind(idx + 1, dp->key);
            insertStatement.bind(idx + 2, static_cast<long long>(dp->timestamp));
            insertStatement.bind(idx + 3, static_cast<int>(dp->type));
            insertStatement.bind(idx + 4, dp->toString());
            idx += 4;
        }
        try {
            insertStatement.exec();
            cacheNumItems += numDataPointsWritten;
        } catch (std::exception& e) {
            SM::getLogger()->warning(fmt::format("[{}] Warning: Failed to cache data point to SQLite database: {}", id, e.what()));
        }
        SM::getLogger()->info(fmt::format("[{}] Wrote {} data points to cache. Cache size: {} items", id, numDataPointsWritten, cacheNumItems));
    }
}

void DataSource::closeCache() {
    writeCacheValues();
    SM::getLogger()->info(fmt::format("[{}] Closed data point cache.", id));
}