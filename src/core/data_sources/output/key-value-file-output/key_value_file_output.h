#ifndef KEY_VALUE_FILE_H
#define KEY_VALUE_FILE_H

#include "core/data_sources/base/file-output/file_output.h"
#include "utility/timer/simple_timer.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

class KeyValueFileOutputDataSource : public FileOutputDataSource {
private:
    std::basic_regex<char> equalsEscapeRegex = std::basic_regex("=");
    uint64_t lastTimestamp;
public:
    KeyValueFileOutputDataSource(const json &definition): FileOutputDataSource(definition) {}
    void update() override;
};

#endif
