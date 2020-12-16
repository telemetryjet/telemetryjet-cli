#include "serial_stream.h"

SerialStreamDataSource::SerialStreamDataSource(const std::string &id, const std::string &type, const json &options)
    : DataSource(id, type) {
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
}

void SerialStreamDataSource::open() {
    serial = std::make_unique<SerialWrapper>(portName, baudRate);
    reconnectTimer = std::make_unique<SimpleTimer>(5000);
    DataSource::open();
}

void SerialStreamDataSource::close() {
    if (isOpen) {
        if (serial->isOpen()) {
            serial->close();
        }

        reconnectTimer.reset();
        DataSource::close();
    }
}

void SerialStreamDataSource::update() {
    if (isOpen) {
        if (serial->isOpen()) {
            serial->poll();
        }

        if (reconnectTimer->check() && !serial->isOpen()) {
            // Periodically try to open serial port
            serial->open();
        }
    }
}

bool SerialStreamDataSource::checkDone() {
    return false;
}

bool SerialStreamDataSource::checkExitOnError() {
    return !isOpen || !serial->isOpen();
}
