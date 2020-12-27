#ifndef TELEMETRYJETCLI_CONSOLE_INPUT_H
#define TELEMETRYJETCLI_CONSOLE_INPUT_H

#include "core/data_source.h"

class ConsoleInputDataSource : public DataSource {
public:
    ConsoleInputDataSource(const json &definition): DataSource(definition) {}
    void open() override;
    void close() override;
    void update() override;
};

#endif