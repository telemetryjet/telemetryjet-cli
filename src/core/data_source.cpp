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

// Write to the queue, to be output to all other data sources on the next iteration
void DataSource::write(std::shared_ptr<DataPoint> dataPoint) {
    out.push_back(dataPoint);
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
