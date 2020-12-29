#include "mqtt_server.h"

MqttServerDataSource::MqttServerDataSource(const json& definition)
    : DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires an options object", id, type));
    }
}

void MqttServerDataSource::open() {
    DataSource::open();
}

void MqttServerDataSource::close() {
    DataSource::close();
}

void MqttServerDataSource::update() {
    DataSource::update();
}
