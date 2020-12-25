#include "joystick.h"

void JoystickDataSource::open() {
    if (SDL_NumJoysticks() <= 0) {
        throw std::runtime_error(fmt::format("[{}] No joysticks were found!", id));
    }

    joy = SDL_JoystickOpen(0);

    if (!joy) {
        throw std::runtime_error(fmt::format("[{}] Failed to open joystick!", id));
        return;
    }

    SM::getLogger()->info(fmt::format("[{}] Opened joystick {}", id,SDL_JoystickName(joy)));

    pollTimer = std::make_unique<SimpleTimer>(10);
    DataSource::open();
}

void JoystickDataSource::close() {
    SDL_JoystickClose(joy);
    pollTimer.reset();
    DataSource::close();
}

void JoystickDataSource::update() {
    pollTimer->wait();

    if (SDL_JoystickGetAttached(joy) == SDL_FALSE) {
        throw std::runtime_error(fmt::format("[{}] joystick is no longer attached!", id));
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
        write(std::make_shared<DataPoint>(fmt::format("axis_{}", axisId), timestamp, axisPositionNormalized));
    }

    int numButtons = SDL_JoystickNumButtons(joy);
    for (int buttonId = 0; buttonId < numButtons; buttonId++) {
        bool_t buttonPosition = SDL_JoystickGetButton(joy, buttonId) == 1;
        write(std::make_shared<DataPoint>(fmt::format("button_{}", buttonId), timestamp, buttonPosition));
    }
}
