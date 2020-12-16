#ifndef CONSOLE_DATA_SOURCE_H
#define CONSOLE_DATA_SOURCE_H

#include "core/data_source.h"

class ConsoleOutputDataSource : public DataSource {
public:
    ConsoleOutputDataSource(std::string id, const json &options);
    void update() override;
    bool checkDone() override {
        return true;
    }
    bool checkExitOnError() override {
        return false;
    }
};

#endif