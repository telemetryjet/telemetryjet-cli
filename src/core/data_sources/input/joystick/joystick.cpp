#include "joystick.h"

JoystickDataSource::JoystickDataSource(const std::string& id, const json &options)
        : DataSource(id, "joystick") {
}

void JoystickDataSource::open() {
    DataSource::open();
}

void JoystickDataSource::close() {
    DataSource::close();
}

void JoystickDataSource::update() {
    if (!isOpen) {
        return;
    }
}
