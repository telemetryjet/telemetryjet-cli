#ifndef TELEMETRYJETCLI_SDL_H
#define TELEMETRYJETCLI_SDL_H

/**
 * SDLWrapper: Handles basic initialization and config of SDL.
 * Used for any user input, such as keyboard/mouse/joystick.
 */
class SDLWrapper {
public:
    static void init();
    static void destroy();
    static bool initialized;
};

#endif