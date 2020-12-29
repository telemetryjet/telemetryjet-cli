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
    if (!options.contains("host") || !options["host"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'host' of type String", id, type));
    }
    host = options["host"];
    if (options.contains("port") && options["port"].is_number_integer()) {
        port = 1883;
    }
    if (options.contains("transport") && options["transport"].is_string()) {
        if (options["transport"] != "tcp" && options["transport"] != "websocket") {
            throw std::runtime_error(fmt::format("[{}] 'transport' must be either 'tcp' or 'websocket'", id));
        }
        transport = options["transport"];
    }
    if (options.contains("tls") && options["tls"].is_object()) {
        json tlsOptions = options["tls"];
        if (!tlsOptions.contains("enabled") || !tlsOptions["enabled"].is_boolean()) {
            throw std::runtime_error(fmt::format("[{}] 'tls' option group requires field 'enabled' of type Boolean", id));
        }
        tlsEnabled = tlsOptions["enabled"];
        if (tlsEnabled) {
            if (!tlsOptions.contains("caCertFile") || !tlsOptions["caCertFile"].is_string()) {
                throw std::runtime_error(fmt::format("[{}] 'tls' option group requires field 'caCertFile' of type String", id));
            }
            caCertFile = tlsOptions["caCertFile"];
        }
    }
}

void MqttClientDataSource::open() {
    SM::getLogger()->info(fmt::format("[{}] Host: {}", id, host));
    SM::getLogger()->info(fmt::format("[{}] Port: {}", id, port));
    SM::getLogger()->info(fmt::format("[{}] Transport: {}", id, transport));
    SM::getLogger()->info(fmt::format("[{}] TLS Enabled: {}", id, tlsEnabled));
    if (tlsEnabled) {
        SM::getLogger()->info(fmt::format("[{}] CA Cert File: {}", id, caCertFile));
    }
    client = MQTT_NS::make_async_client();
    DataSource::open();
}

void MqttClientDataSource::close() {
    DataSource::close();
}

void MqttClientDataSource::update() {
    DataSource::update();
}
