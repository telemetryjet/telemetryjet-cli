#include "console_input.h"
#include <string>
#include <iostream>
#include <sstream>
#include "services/service_manager.h"
#include "utility/key_value/key_value_utils.h"

void ConsoleInputDataSource::open() {
    DataSource::open();
}

void ConsoleInputDataSource::update() {
    DataSource::update();
    std::string newInput;
    std::getline(std::cin, newInput);
    try {
        std::pair<std::string, std::string> keyPair = parseKeyValueString(newInput);
        uint64_t timestamp = getCurrentMillis();
        write(createDataPointFromString(keyPair.first, timestamp, keyPair.second));
    } catch (std::exception &e) {
        SM::getLogger()->warning(fmt::format("[{}] Failed to parse input line: [{}]", id, newInput));
    }
}
