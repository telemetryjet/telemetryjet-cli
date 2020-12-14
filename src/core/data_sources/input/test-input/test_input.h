#ifndef TEST_DATA_SOURCE_H
#define TEST_DATA_SOURCE_H

#include "core/data_source.h"
#include "utility/timer/simple_timer.h"

class TestInputDataSource : public DataSource {
private:
    float frequency = 0.001;
    float amplitude = 1000;
    float offset = 1500;
    std::string key;
    uint64_t interval = 10;
    SimpleTimer* timer;
public:
    TestInputDataSource(std::string id, const json &options);
    ~TestInputDataSource() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif