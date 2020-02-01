#include "deviceManager.hpp"

DeviceManager::DeviceManager() {
    _paused = false;
}

void DeviceManager::update() {
    // log("Polling devices...");

    uint64_t startTime = (system_clock::now().time_since_epoch()).count();

    struct sp_port** ports;
    sp_return        listPortResult = sp_list_ports(&ports);

    portList.clear();
    
    std::set<std::string> s;
    s.insert("/dev/cu.Bluetooth-Incoming-Port");

    if (listPortResult == SP_OK) {
        for (int i = 0; ports[i]; i++) {
            std::string portName(sp_get_port_name(ports[i]));
            portList.insert(portName);

            if (s.find(portName) == s.end()) {
                if (!deviceList.count(portName)) {
                    log("Connecting to port " + portName);
                    // Found a device that doesn't exist yet
                    deviceList.emplace(std::make_pair(portName, Device(portName)));
                }
            }
        }
        sp_free_port_list(ports);
    }
    uint64_t delta = (system_clock::now().time_since_epoch()).count() - startTime;

    // log("Polling devices took " + std::to_string(delta / 1000.0f) + "ms");

    // Check if any existing devices have been closed and remove them
    // This is the case if any of the following have happened:
    // - error in serial read
    // - The device doesn't show up in port list
    auto iterator = deviceList.cbegin();
    while (iterator != deviceList.cend()) {
        Device device = iterator->second;
        if (!device.isOpen() || !portList.count(iterator->first)) {
            device.close();
            iterator = deviceList.erase(iterator);
        } else {
            iterator++;
        }
    }
}

void DeviceManager::getData() {
    for (const auto& deviceItem : deviceList) {
        std::string portName = deviceItem.first;
        Device      device   = deviceItem.second;
        device.getData();
    }
}

void DeviceManager::log(std::string message) {
    std::cout << "[Device Manager] " << message << "\n";
}

void DeviceManager::disconnectDevices() {
    for (const auto& deviceItem : deviceList) {
        std::string portName = deviceItem.first;
        Device      device   = deviceItem.second;
        log("Disconnecting from port " + portName);
        device.close();
    }
}

void DeviceManager::shutdown() {
    disconnectDevices();
}

void DeviceManager::setPaused(bool paused) {
    if (paused) {
        log("Pausing device manager and disconnecting from all devices...");
        // Disconnect from all devices when paused
        disconnectDevices();
    } else {
        log("Resuming device manager...");
        _paused = false;
    }
}
