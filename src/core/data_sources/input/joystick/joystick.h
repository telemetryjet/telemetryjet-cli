#ifndef TELEMETRYJETCLI_JOYSTICK_H
#define TELEMETRYJETCLI_JOYSTICK_H

#include "core/data_source.h"
#include <SDL.h>
#include "utility/timer/simple_timer.h"

class JoystickDataSource: public DataSource {
private:
    SDL_Joystick* joy;
    std::unique_ptr<SimpleTimer> pollTimer;
    std::unique_ptr<SimpleTimer> reconnectTimer;
    bool isJoystickOpen = false;
    void openJoystick();
public:
    JoystickDataSource(const json &definition): DataSource(definition) {}
    void open() override;
    void close() override;
    void update() override;
};

#endif