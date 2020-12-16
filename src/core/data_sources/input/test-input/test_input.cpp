#include "test_input.h"

TestInputDataSource::TestInputDataSource(std::string id, const json& options) : DataSource(id, "test-input") {
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
    if (isOpen && timer->check()) {
        uint64_t timestamp = getCurrentMillis();
        float newValue = (float)(sin((double)timestamp * ((double)frequency)) * amplitude) + offset + ((rand() % 1000 ) / 100.0);
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.{}",id,key), newValue, timestamp));
    }
}

void TestInputDataSource::close() {
    if (isOpen) {
        timer.reset();
        DataSource::close();
    }
}