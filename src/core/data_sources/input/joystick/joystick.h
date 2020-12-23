#ifndef TELEMETRYJETCLI_JOYSTICK_H
#define TELEMETRYJETCLI_JOYSTICK_H

#include "core/data_source.h"
#include <SDL.h>
#include "utility/timer/simple_timer.h"

class JoystickDataSource: public DataSource {
private:
    SDL_Joystick* joy;
    std::unique_ptr<SimpleTimer> pollTimer;
public:
    JoystickDataSource(const std::string& id, const json &options);
    void open() override;
    void close() override;
    void update() override;
    bool checkDone() override {
        return false;
    }
    bool checkExitOnError() override {
        return false;
    }
};

#endif