#include "console_output.h"

#include <utility>

ConsoleOutputDataSource::ConsoleOutputDataSource(std::string id, const json& options)
    : DataSource(std::move(id), "console-output") {
}

void ConsoleOutputDataSource::update() {
    if (!in.empty()) {
        for (auto& inValue : in) {
            std::cout << fmt::format("{}={}\n",inValue->key, inValue->value);
        }
    }
}