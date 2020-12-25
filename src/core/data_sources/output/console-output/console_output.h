#ifndef CONSOLE_DATA_SOURCE_H
#define CONSOLE_DATA_SOURCE_H

#include "core/data_source.h"

class ConsoleOutputDataSource : public DataSource {
public:
    ConsoleOutputDataSource(const json &definition): DataSource(definition) {}
    void open() override;
    void update() override;
};

#endif