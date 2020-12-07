#ifndef CONSOLE_DATA_SOURCE_H
#define CONSOLE_DATA_SOURCE_H

#include "core/data_source.h"

class ConsoleDataSource : public DataSource {
public:
    ConsoleDataSource(std::string id, const json &options);
    ~ConsoleDataSource() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif