#ifndef TELEMETRYJETCLI_TEST_DATA_SOURCE_H
#define TELEMETRYJETCLI_TEST_DATA_SOURCE_H

#include "core/data_source.h"
#include "utility/simple_timer.h"

class TestDataSource : public DataSource {
private:
    float frequency = 100;
    float amplitude = 100;
    float offset = 0;
    std::string key;
    uint64_t interval = 10;
    SimpleTimer* timer;
public:
    TestDataSource(std::string id, const json &options);
    ~TestDataSource() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif //TELEMETRYJETCLI_TEST_DATA_SOURCE_H
