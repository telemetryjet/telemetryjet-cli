#ifndef TELEMETRYJETCLI_DATA_SOURCE_NETWORK_H
#define TELEMETRYJETCLI_DATA_SOURCE_NETWORK_H

#include "data_source.h"
#include <nlohmann/json.hpp>
#include <memory>
using json = nlohmann::json;

class Network {
private:
    std::vector<std::shared_ptr<DataSource>> dataSources;
    std::vector<std::pair<std::string, std::shared_ptr<DataPoint>>> valueCache;
public:
    Network(const json& definitions);
    ~Network();
    void start();
    void update();
    void stop();
    bool isOpen();
};

#endif //TELEMETRYJETCLI_DATA_SOURCE_NETWORK_H
