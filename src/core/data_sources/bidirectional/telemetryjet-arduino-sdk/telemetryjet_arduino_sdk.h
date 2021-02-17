#ifndef TELEMETRYJETCLI_TELEMETRYJET_ARDUINO_SDK_H
#define TELEMETRYJETCLI_TELEMETRYJET_ARDUINO_SDK_H

#include "core/data_source.h"
#include "core/data_sources/base/serial-stream/serial_stream.h"
#include "utility/serial/serial_wrapper.h"
#include <memory>

class TelemetryJetArduinoSdkDataSource: public SerialStreamDataSource {
private:
    std::map<std::string, uint16_t> dimensions;

    // Fixed input, output, and temporary buffers
    uint8_t tempBuffer[32];
    uint8_t rxBuffer[32];
    uint8_t txBuffer[32];
    uint8_t rxIndex;
    uint8_t txIndex;
    uint32_t numDroppedRxPackets;
    uint32_t numRxPackets;
    uint32_t numTxPackets;
public:
    TelemetryJetArduinoSdkDataSource(const json &definition): SerialStreamDataSource(definition) {
        if (!options.contains("dimensions") || !options["dimensions"].is_object()) {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'dimensions' of type 'object'", id, type));
        }
        for (auto& el : options["dimensions"].items()) {
            auto& key = el.key();
            auto& val = el.value();
            if (!val.is_number_integer()) {
                throw std::runtime_error(fmt::format("[{}] data source type '{}' requires all mappings in 'dimensions' to be string -> int", id, type));
            }
            dimensions.insert_or_assign(key, val);
        }
    }
    void update() override;
};

#endif