#include "sdl.h"
#include <SDL.h>
#include "services/service_manager.h"
#include <fmt/format.h>

bool SDLWrapper::initialized = false;

void SDLWrapper::init() {
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    int status = SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
    if (status >= 0) {
        SM::getLogger()->info("Input subsystem initialized.");
        initialized = true;
    } else {
        SM::getLogger()->error("Failed to initialize input subsystem. Any joystick data sources will not function.");
        return;
    }

    SM::getLogger()->info(fmt::format("Found {} joysticks.", SDL_NumJoysticks()));

    for(int i = 0; i < SDL_NumJoysticks(); i++) {
        SM::getLogger()->info(fmt::format("{}: {}", i, SDL_JoystickNameForIndex(i)));
    }
}

void SDLWrapper::destroy() {
    if (initialized) {
        SDL_Quit();
    }
}
