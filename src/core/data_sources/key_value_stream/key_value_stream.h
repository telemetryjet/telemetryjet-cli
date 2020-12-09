#ifndef KEY_VALUE_STREAM_H
#define KEY_VALUE_STREAM_H

#include "core/data_source.h"

class KeyValueStream: public DataSource {
public:
    KeyValueStream(std::string id, const json &options);
    ~KeyValueStream() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif
