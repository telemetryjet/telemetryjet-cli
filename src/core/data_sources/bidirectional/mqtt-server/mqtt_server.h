#ifndef TELEMETRYJETCLI_MQTT_SERVER_H
#define TELEMETRYJETCLI_MQTT_SERVER_H

#include "core/data_source.h"

class MqttServerDataSource: public DataSource {
private:
public:
    explicit MqttServerDataSource(const json &definition);
    void open() override;
    void close() override;
    void update() override;
};

#endif