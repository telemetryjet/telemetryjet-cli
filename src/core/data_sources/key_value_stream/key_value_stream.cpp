#include "key_value_stream.h"

KeyValueStream::KeyValueStream(std::string id, const json &options) : DataSource(id, "key-value-stream") {

}

KeyValueStream::~KeyValueStream() {

}

void KeyValueStream::close() {

}

void KeyValueStream::open() {

}

void KeyValueStream::update() {

}

bool KeyValueStream::isOpen() {
    return false;
}
