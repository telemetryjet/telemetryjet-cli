#include "serial_stream.h"

SerialStreamDataSource::SerialStreamDataSource(const std::string &id, const std::string &type, const json &options)
    : DataSource(id, type) {

}

SerialStreamDataSource::~SerialStreamDataSource() {

}

void SerialStreamDataSource::open() {

}

void SerialStreamDataSource::close() {

}

bool SerialStreamDataSource::isOpen() {
    return false;
}
