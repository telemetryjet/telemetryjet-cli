#include "joystick.h"

void JoystickDataSource::openJoystick() {
    isJoystickOpen = false;
    if (SDL_NumJoysticks() <= 0) {
        SM::getLogger()->warning(fmt::format("[{}] Could not connect to a joystick! Will retry in 5000ms", id));
        return;
    }

    joy = SDL_JoystickOpen(0);

    if (!joy) {
        SM::getLogger()->warning(fmt::format("[{}] Could not connect to a joystick! Will retry in 5000ms", id));
        return;
    }

    isJoystickOpen = true;
    SM::getLogger()->info(fmt::format("[{}] Opened joystick {}", id,SDL_JoystickName(joy)));
}

void JoystickDataSource::open() {
    pollTimer = std::make_unique<SimpleTimer>(10);
    reconnectTimer = std::make_unique<SimpleTimer>(5000);
    DataSource::open();
}

void JoystickDataSource::close() {
    SDL_JoystickClose(joy);
    reconnectTimer.reset();
    pollTimer.reset();
    DataSource::close();
}

void JoystickDataSource::update() {
    if (isJoystickOpen) {
        pollTimer->wait();

        if (SDL_JoystickGetAttached(joy) == SDL_FALSE) {
            isJoystickOpen = false;
            SM::getLogger()->warning(fmt::format("[{}] joystick is no longer attached!", id));
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
            write(std::make_shared<DataPoint>(fmt::format("axis_{}", axisId), timestamp, axisPositionNormalized));
        }

        int numButtons = SDL_JoystickNumButtons(joy);
        for (int buttonId = 0; buttonId < numButtons; buttonId++) {
            bool_t buttonPosition = SDL_JoystickGetButton(joy, buttonId) == 1;
            write(std::make_shared<DataPoint>(fmt::format("button_{}", buttonId), timestamp, buttonPosition));
        }
    } else {
        reconnectTimer->wait();
        openJoystick();
    }
}
