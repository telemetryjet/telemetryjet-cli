#include "mqtt_client.h"
/**
 * "host"
 * "port"
 * "transport": "tcp", "websocket"
 * "tls":
 *  - "caCertFile"
 *  - "certFile"
 *  - "privateKeyFile"
 */
MqttClientDataSource::MqttClientDataSource(const json& definition)
    : DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires an options object", id, type));
    }
}

void MqttClientDataSource::open() {
    DataSource::open();
}

void MqttClientDataSource::close() {
    DataSource::close();
}

void MqttClientDataSource::update() {
    DataSource::update();
}
