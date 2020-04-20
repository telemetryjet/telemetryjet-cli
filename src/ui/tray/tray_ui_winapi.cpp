#include "tray_ui.h"
#include <services/service_manager.h>

bool TrayUI::shouldQuit = false;

void TrayUI::updateMenu() {
}

void TrayUI::init() {
    SM::getLogger()->info("Started Tray UI.");
}

void TrayUI::update() {
}

void TrayUI::destroy() {
    SM::getLogger()->info("Stopped Tray UI.");
}