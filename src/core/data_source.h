#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "data_point.h"
#include <vector>
#include <nlohmann/json.hpp>
#include "fmt/format.h"
#include "services/service_manager.h"
#include "utility/time_utils.h"

using json = nlohmann::json;

class DataSource {
private:
public:
    const std::string id;
    const std::string type;
    const std::string prefix;
    std::vector<std::shared_ptr<DataPoint>> in;
    std::vector<std::shared_ptr<DataPoint>> out;
    DataSource(std::string id, std::string type) : id(id), type(type), prefix(id) {};
    virtual ~DataSource() = default;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void update() = 0;
    virtual bool isOpen() = 0;
};

#endif //TELEMETRYJETCLI_DATA_SOURCE_H
