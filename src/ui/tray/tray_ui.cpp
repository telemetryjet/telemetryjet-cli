#include "tray_ui.h"
#include <services/service_manager.h>
#include <utility/time_utils.h>

bool TrayUI::shouldQuit = false;

void StartSystemCallback(struct tray_menu *item) {
    SM::getDeviceManager()->start();
    TrayUI::updateMenu();
}

void StopSystemCallback(struct tray_menu *item) {
    SM::getDeviceManager()->stop();
    TrayUI::updateMenu();
}

void QuitCallback(struct tray_menu *item) {
    TrayUI::shouldQuit = true;
}

struct tray TrayUI::trayDefinition = {
        .icon = "icon2.png",
        .menu = (struct tray_menu[]){{"TelemetryJet Server v0.1.0", 1, 0, NULL, NULL},
                                     {"-", 0, 0, NULL, NULL},
                                     {"Quit", 0, 0, QuitCallback, NULL},
                                     {NULL, 0, 0, NULL, NULL}},
};

void TrayUI::updateMenu() {
    bool systemsRunning = SM::getDeviceManager()->isRunning;

    std::string toggleRunningString = systemsRunning ? "Stop System" : "Start System";

    TrayUI::trayDefinition.menu = (struct tray_menu[]){{"TelemetryJet Server v0.1.0", 1, 0, NULL, NULL},
                                                       {"-", 0, 0, NULL, NULL},
                                                       {toggleRunningString.data(), 0, 0, NULL, NULL},
                                                       {"-", 0, 0, NULL, NULL},
                                                       {getTimestamp().data(), 0, 0, NULL, NULL},
                                                       {"Quit", 0, 0, QuitCallback, NULL},
                                                       {NULL, 0, 0, NULL, NULL}};
    tray_update(&TrayUI::trayDefinition);
}

void TrayUI::init() {
    tray_init(&TrayUI::trayDefinition);
    //TrayUI::updateMenu();
}


void TrayUI::update() {
    tray_loop(0);
}

void TrayUI::destroy() {
    // Remove tray icon.
    tray_exit();
}