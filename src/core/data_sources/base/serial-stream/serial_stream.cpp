#include "serial_stream.h"

SerialStreamDataSource::SerialStreamDataSource(const json &definition): DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires an options object.", type, id));
    }
    if (!options.contains("port") || !options["port"].is_string()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires option 'port' of type String.", type, id));
    }
    if (!options.contains("baudrate") || !options["baudrate"].is_number_integer()) {
        throw std::runtime_error(fmt::format("{} data source '{}' requires option 'baudrate' of type Integer.", type, id));
    }
    portName = options["port"];
    baudRate = options["baudrate"];
    assertDependency("serialport", portName, fmt::format("Multiple data sources cannot share the same serial port: {}", portName));
}

void SerialStreamDataSource::open() {
    serial = std::make_unique<SerialWrapper>(portName, baudRate);
    reconnectTimer = std::make_unique<SimpleTimer>(5000);
    DataSource::open();
}

void SerialStreamDataSource::close() {
    if (serial->isOpen()) {
        serial->close();
    }

    reconnectTimer.reset();
    DataSource::close();
}

void SerialStreamDataSource::update() {
    if (serial->isOpen()) {
        error = false;
        serial->pollBlocking();
    }

    if (reconnectTimer->check() && !serial->isOpen()) {
        // Periodically try to open serial port
        serial->open();
        // Set error flag if we are unable to connect
        error = !serial->isOpen();
    }
}