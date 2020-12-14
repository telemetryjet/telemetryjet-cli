#ifndef CONSOLE_DATA_SOURCE_H
#define CONSOLE_DATA_SOURCE_H

#include "core/data_source.h"

class ConsoleOutputDataSource : public DataSource {
public:
    ConsoleOutputDataSource(std::string id, const json &options);
    ~ConsoleOutputDataSource() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif