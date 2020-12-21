#include "joystick.h"

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
