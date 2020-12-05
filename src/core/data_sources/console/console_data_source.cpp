#include "console_data_source.h"

ConsoleDataSource::ConsoleDataSource(std::string id, const json& options) : DataSource(id, "console") {
}

ConsoleDataSource::~ConsoleDataSource() = default;

void ConsoleDataSource::open() {

}

void ConsoleDataSource::update() {
    if (!in.empty()) {
        for (auto& inValue : in) {
            std::cout << fmt::format("{}={}\n",inValue->key, inValue->value);
        }
    }
}

void ConsoleDataSource::close() {

}

bool ConsoleDataSource::isOpen() {
    return true;
}
