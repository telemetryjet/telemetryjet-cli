#ifndef KEY_VALUE_STREAM_H
#define KEY_VALUE_STREAM_H

#include "core/data_source.h"
#include <memory>
#include "utility/serial/serial_wrapper.h"
#include "utility/timer/simple_timer.h"
#include "core/data_sources/base/serial-stream/serial_stream.h"

class KeyValueStream: public SerialStreamDataSource {
private:
    std::list<uint8_t> keyValueBuffer;
public:
    KeyValueStream(const std::string& id, const json &options);
    ~KeyValueStream() override;
    void open() override;
    void close() override;
    void update() override;
};

#endif
