#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "data_point.h"
#include <utility>
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
    std::vector<std::shared_ptr<DataPoint>> in;
    std::vector<std::shared_ptr<DataPoint>> out;
    bool isOpen = false;
    DataSource(std::string id, std::string type) : id(std::move(id)), type(std::move(type)) {};
    virtual ~DataSource() = default;
    virtual void open() {
        isOpen = true;
    }
    virtual void close() {
        isOpen = false;
    }
    virtual void update() {};
    virtual bool checkDone() {
        return false;
    }
    virtual bool checkExitOnError() {
        return false;
    }
};

#endif
