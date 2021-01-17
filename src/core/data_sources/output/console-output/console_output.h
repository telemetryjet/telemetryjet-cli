#ifndef CONSOLE_DATA_SOURCE_H
#define CONSOLE_DATA_SOURCE_H

#include "core/data_source.h"

class ConsoleOutputDataSource : public DataSource {
public:
    ConsoleOutputDataSource(const json &definition): DataSource(definition) {
        assertDependency("console", "", fmt::format("[{}] Multiple data sources cannot share use of standard input/output.", id));
        inputEnabled = false;
    }
    void open() override;
    void update() override;
};

#endif