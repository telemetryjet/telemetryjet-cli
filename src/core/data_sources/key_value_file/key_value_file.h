#ifndef KEY_VALUE_FILE_H
#define KEY_VALUE_FILE_H

#include "core/data_source.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include "utility/timer/simple_timer.h"

class KeyValueFile : public DataSource {
private:
    std::ofstream outputFile;
    std::string filename;
    std::ios::openmode mode;
    std::string modeString;
    bool generateNewFile;
    SimpleTimer* flushTimer;
    std::basic_regex<char> equalsEscapeRegex;
public:
    KeyValueFile(std::string id, const json &options);
    ~KeyValueFile() override;
    void open() override;
    void close() override;
    void update() override;
    bool isOpen() override;
};

#endif
