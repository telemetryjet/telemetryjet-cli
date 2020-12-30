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
MqttClientDataSource::MqttClientDataSource(const json& definition): DataSource(definition) {
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
    auto& client;
    if (tlsEnabled) {
        SM::getLogger()->info(fmt::format("[{}] CA Cert File: {}", id, caCertFile));
        if (transport == "tcp") {
            client = MQTT_NS::make_tls_async_client(ioContext, host, port);
        } else {
            client = MQTT_NS::make_tls_async_client_ws(ioContext, host, port);
        }
    } else {
        if (transport == "tcp") {
            client = MQTT_NS::make_async_client(ioContext, host, port);
        } else {
            client = MQTT_NS::make_async_client_ws(ioContext, host, port);
        }
    }
    using packet_id_t = typename std::remove_reference_t<decltype(*client)>::packet_id_t;

    // Setup client
    client->set_client_id("cid1");
    client->set_clean_session(true);
    if (tlsEnabled) {
        client->get_ssl_context().load_verify_file(caCertFile);
    }

    // Setup handlers
    client->set_connack_handler([&] (bool sp, MQTT_NS::connect_return_code connack_return_code) {
      if (connack_return_code == MQTT_NS::connect_return_code::accepted) {
          // Connection successful
          SM::getLogger()->info(fmt::format("[{}] Connected to MQTT server", id));
          error = false;
      } else {
          // Connection failed
          SM::getLogger()->warning(fmt::format("[{}] Failed to connect to MQTT server", id));
          error = true;
      }
      return true;
    });

    client->set_close_handler([&] () {
        SM::getLogger()->info(fmt::format("[{}] Disconnected from MQTT server", id));
    });

    client->set_error_handler([&] (MQTT_NS::error_code ec) {
        SM::getLogger()->info(fmt::format("[{}] Error in MQTT client: {}", id, ec.message()));
    });

    client->connect();
    DataSource::open();
}

void MqttClientDataSource::close() {
    //client->disconnect();
    //client.reset();
    DataSource::close();
}

void MqttClientDataSource::update() {
    //ioc.poll();
    DataSource::update();
}
