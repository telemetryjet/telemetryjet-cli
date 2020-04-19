#ifndef TELEMETRYSERVER_TRAY_UI_H
#define TELEMETRYSERVER_TRAY_UI_H
#include <tray.h>

class TrayUI {
private:
    static struct tray trayDefinition;
public:
    static bool shouldQuit;
    static void init();
    static void update();
    static void destroy();
    static void updateMenu();
};

#endif //TELEMETRYSERVER_TRAY_UI_H
