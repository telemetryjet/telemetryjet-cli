#include "console_output.h"

#include <utility>

ConsoleOutputDataSource::ConsoleOutputDataSource(std::string id, const json& options)
    : DataSource(std::move(id), "console-output") {
}

void ConsoleOutputDataSource::update() {
    if (!in.empty()) {
        std::string out;
        for (auto& inValue : in) {
            out += fmt::format("{}={}\n",inValue->key, inValue->toString());
        }
        std::cout << out;
    }
}