#include "test_input.h"
#include <random>

TestInputDataSource::TestInputDataSource(const json& definition): DataSource(definition) {
    if (options.is_object()) {
        if (options.contains("frequency") && options["frequency"].is_number()) {
            frequency = options["frequency"];
        }
        if (options.contains("amplitude") && options["amplitude"].is_number()) {
            amplitude = options["amplitude"];
        }
        if (options.contains("offset") && options["offset"].is_number()) {
            offset = options["offset"];
        }
        if (options.contains("interval") && options["interval"].is_number_integer()) {
            interval = options["interval"];
        }
        if (options.contains("key") && options["key"].is_string()) {
            key = options["key"];
        }
    }
}

void TestInputDataSource::open() {
    timer = std::make_unique<SimpleTimer>(interval);
    DataSource::open();
}

void TestInputDataSource::update() {
    timer->wait();

    uint64_t timestamp = getCurrentMillis();
    float64_t newValue = (sin((float64_t)timestamp * (float64_t)frequency) * amplitude) + offset + ((std::rand() % 1000 ) / 100.0);
    write(std::make_shared<DataPoint>(key, timestamp, newValue));
}

void TestInputDataSource::close() {
    timer.reset();
    DataSource::close();
}