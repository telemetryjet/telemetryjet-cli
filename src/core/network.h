#ifndef DATA_SOURCE_NETWORK_H
#define DATA_SOURCE_NETWORK_H

#include "data_point.h"
#include "data_source.h"
#include <memory>
#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include "utility/timer/simple_timer.h"

using json = nlohmann::json;

class Network {
private:
    std::vector<std::shared_ptr<DataSource>> dataSources;
    std::vector<std::shared_ptr<boost::thread>> dataSourceWorkerThreads;
    std::vector<std::pair<std::string, std::shared_ptr<DataPoint>>> valueCache;
    const bool errorMode = false;
    std::shared_ptr<SimpleTimer> dataSourceCheckTimer;
public:
    std::atomic<bool> interrupted = false;
    std::atomic<bool> error = false;
    Network(const json& definitions, bool errorMode);
    void start();
    void stop();
    bool isInitialized();
    bool isDone();
    void propagateDataPoints(std::shared_ptr<DataSource> source);
    void checkDataSources();
};

#endif