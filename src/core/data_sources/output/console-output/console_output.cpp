#include "console_output.h"

ConsoleOutputDataSource::ConsoleOutputDataSource(std::string id, const json& options) : DataSource(id, "console-output") {
}

ConsoleOutputDataSource::~ConsoleOutputDataSource() = default;

void ConsoleOutputDataSource::open() {

}

void ConsoleOutputDataSource::update() {
    if (!in.empty()) {
        for (auto& inValue : in) {
            std::cout << fmt::format("{}={}\n",inValue->key, inValue->value);
        }
    }
}

void ConsoleOutputDataSource::close() {

}

bool ConsoleOutputDataSource::isOpen() {
    return true;
}
