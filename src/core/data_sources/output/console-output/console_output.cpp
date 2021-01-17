#include "console_output.h"

#include <utility>

void ConsoleOutputDataSource::update() {
    if (!in.empty()) {
        std::string out;
        for (auto& inValue : in) {
            out += fmt::format("{}={}\n",inValue->key, inValue->toString());
        }
        std::cout << out;
    }
}

void ConsoleOutputDataSource::open() {
    DataSource::open();
}
