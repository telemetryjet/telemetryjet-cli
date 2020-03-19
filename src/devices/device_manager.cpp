#include "device_manager.h"
#include "services/service_manager.h"

DeviceManager::~DeviceManager() {
    stop();
}

void DeviceManager::start() {
    if (!isRunning) {
        SM::getLogger()->alert("Started device manager!");
        isRunning = true;
    }
}

void DeviceManager::update() {

}

void DeviceManager::stop() {
    if (isRunning) {
        SM::getLogger()->alert("Stopped device manager!");
        isRunning = false;
    }
}