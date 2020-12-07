#include "test_data_source.h"

TestDataSource::TestDataSource(std::string id, const json& options) : DataSource(id, "test") {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("test data source '{}' requires an options object!", id));
    }
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
    } else {
        key = "value";
    }
}

TestDataSource::~TestDataSource() = default;

void TestDataSource::open() {
    timer = new SimpleTimer(interval);
}

void TestDataSource::update() {
    if (timer->check()) {
        uint64_t timestamp = getCurrentMillis();
        float newValue = (float)(sin((double)timestamp * ((double)frequency)) * amplitude) + offset + ((rand() % 1000 ) / 100.0);
        out.push_back(std::make_shared<DataPoint>(fmt::format("{}.{}",prefix,key), newValue, timestamp));
    }
}

void TestDataSource::close() {
    delete timer;
}

bool TestDataSource::isOpen() {
    return true;
}
