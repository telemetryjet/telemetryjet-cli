#ifndef TELEMETRYSERVER_TRAY_UI_H
#define TELEMETRYSERVER_TRAY_UI_H

struct tray_menu;

struct tray {
    char *icon;
    struct tray_menu *menu;
};

struct tray_menu {
    char *text;
    int disabled;
    int checked;

    void (*cb)(struct tray_menu *);
    void *context;

    struct tray_menu *submenu;
};

class TrayUI {
public:
    static bool shouldQuit;
    static void init();
    static void update();
    static void destroy();
    static void updateMenu();
};

#endif //TELEMETRYSERVER_TRAY_UI_H
