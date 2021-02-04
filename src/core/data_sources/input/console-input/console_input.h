#ifndef TELEMETRYJETCLI_CONSOLE_INPUT_H
#define TELEMETRYJETCLI_CONSOLE_INPUT_H

#include "core/data_source.h"

class ConsoleInputDataSource : public DataSource {
public:
    ConsoleInputDataSource(const json &definition): DataSource(definition) {
        assertDependency("console", "", fmt::format("[{}] Multiple data sources cannot share use of standard input/output.", id));
        outputEnabled = false;
    }
    void open() override;
    void update() override;
};

#endif