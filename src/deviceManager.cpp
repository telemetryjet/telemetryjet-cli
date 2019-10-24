#include "deviceManager.hpp"

void DeviceManager::pollDevices() {
    std::cout << "Polling devices...\n";

    uint64_t startTime = (system_clock::now().time_since_epoch()).count();

    struct sp_port** ports;
    sp_return        listPortResult = sp_list_ports(&ports);

    if (listPortResult == SP_OK) {
        std::cout << "== Serial Ports ==\n";
        for (int i = 0; ports[i]; i++) {
            std::string portName(sp_get_port_name(ports[i]));
            std::cout << i << ") " << portName << "\n";
            if (!deviceList.count(portName)) {
                // Found a device that doesn't exist yet
                deviceList.emplace(std::make_pair(portName, Device(portName)));
            }
        }
        sp_free_port_list(ports);
    } else {
        std::cout << "Found no serial ports!\n";
    }
    uint64_t delta = (system_clock::now().time_since_epoch()).count() - startTime;

    std::cout << "took " << delta / 1000.0f << "ms \n\n";
}

void DeviceManager::shutdown() {
    for (const auto& deviceItem : deviceList) {
        std::string portName = deviceItem.first;
        Device      device   = deviceItem.second;
        device.close();
    }
}