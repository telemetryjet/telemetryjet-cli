#include "deviceManager.hpp"

void DeviceManager::pollDevices() {
    std::cout << "Polling devices...\n";

    uint64_t startTime = (system_clock::now().time_since_epoch()).count();

    struct sp_port** ports;
    sp_return        listPortResult = sp_list_ports(&ports);

    if (listPortResult == SP_OK) {
        std::cout << "== Serial Ports ==\n";
        for (int i = 0; ports[i]; i++) {
            std::cout << i << ") " << sp_get_port_name(ports[i]) << "\n";
        }
        sp_free_port_list(ports);
    } else {
        std::cout << "Found no serial ports!\n";
    }
    uint64_t delta = (system_clock::now().time_since_epoch()).count() - startTime;

    std::cout << "took " << delta / 1000.0f << "ms \n\n";
}
