#ifndef TELEMETRYJETCLI_KEY_VALUE_FILE_INPUT_H
#define TELEMETRYJETCLI_KEY_VALUE_FILE_INPUT_H

#include "core/data_sources/base/file-input/file_input.h"
#include "utility/key_value/key_value_utils.h"
#include <list>

class KeyValueFileInputDataSource : public FileInputDataSource {
private:
    std::list<uint8_t> keyValueBuffer;
public:
    KeyValueFileInputDataSource(const json &definition): FileInputDataSource(definition) {}
    void update() override;
};

#endif