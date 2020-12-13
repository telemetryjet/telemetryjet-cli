#ifndef KEY_VALUE_FILE_H
#define KEY_VALUE_FILE_H

#include "core/data_sources/file_data_source.h"
#include "utility/timer/simple_timer.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

class KeyValueFile : public OutputFileDataSource {
private:
    std::basic_regex<char> equalsEscapeRegex;

public:
    KeyValueFile(const std::string& id, const json& options);
    ~KeyValueFile() override;
    void update() override;
};

#endif
