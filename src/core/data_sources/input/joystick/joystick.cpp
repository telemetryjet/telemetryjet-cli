#include "joystick.h"

JoystickDataSource::JoystickDataSource(const std::string& id, const json &options)
        : DataSource(id, "joystick") {
}

void JoystickDataSource::open() {
    if (SDL_NumJoysticks() <= 0) {
        SM::getLogger()->error(fmt::format("{}: No joysticks were found!", id));
        return;
    }

    joy = SDL_JoystickOpen(0);

    if (!joy) {
        SM::getLogger()->error(fmt::format("{}: Failed to open joystick!", id));
        return;
    }

    SM::getLogger()->info(fmt::format("{}: Opened joystick {}.", id,SDL_JoystickName(joy)));

    pollTimer = std::make_unique<SimpleTimer>(10);
    DataSource::open();
}

void JoystickDataSource::close() {
    if (isOpen) {
        SDL_JoystickClose(joy);
        pollTimer->reset();
        DataSource::close();
    }
}

void JoystickDataSource::update() {
    if (!isOpen) {
        return;
    }


    if (pollTimer->check()) {
        if (SDL_JoystickGetAttached(joy) == SDL_FALSE) {
            close();
            return;
        }
        uint64_t timestamp = getCurrentMillis();
        int numAxes = SDL_JoystickNumAxes(joy);
        for (int axisId = 0; axisId < numAxes; axisId++) {
            int16_t axisPositionInt = SDL_JoystickGetAxis(joy, axisId);
            float64_t axisPositionNormalized = axisPositionInt / 32767.0;
            if (axisPositionNormalized < -1.0) {
                axisPositionNormalized = -1.0;
            }
            if (axisPositionNormalized > 1.0) {
                axisPositionNormalized = 1.0;
            }
            out.push_back(std::make_shared<DataPoint>(fmt::format("{}.axis_{}",id, axisId), timestamp, axisPositionNormalized));
        }

        int numButtons = SDL_JoystickNumButtons(joy);
        for (int buttonId = 0; buttonId < numButtons; buttonId++) {
            bool_t buttonPosition = SDL_JoystickGetButton(joy, buttonId) == 1;
            out.push_back(std::make_shared<DataPoint>(fmt::format("{}.button_{}",id, buttonId), timestamp, buttonPosition));
        }
    }
}
