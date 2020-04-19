#include "device_manager.h"
#include "devices/protocols/system_usage/system_usage_device.h"
#include "fmt/format.h"
#include "model/records.h"
#include "protocols/nmea_0183/nmea_0183_device.h"
#include "services/service_manager.h"

DeviceManager::DeviceManager() {
    timer = new SimpleTimer(100);
}

DeviceManager::~DeviceManager() {
    stop();
}

void DeviceManager::start() {
    if (!isRunning) {
        SM::getLogger()->alert("Started device manager!");
        record_log_t::createLog(fmt::format("Started devices."));

        // Load the device list and initialize instances
        std::vector<record_device_t> deviceDefinitions = record_device_t::getDevices();
        SM::getLogger()->alert("=== CONFIGURING DEVICES ===");
        for (auto& deviceDefinition : deviceDefinitions) {
            SM::getLogger()->alert(
                fmt::format("- Device definition: [name={}]", deviceDefinition.name));
            record_log_t::createLog(
                fmt::format("Started device (address={})", deviceDefinition.name));

            Device* newDevice;
            switch (deviceDefinition.protocol) {
            case NMEA_0183:
                newDevice = new Nmea0183Device();
                break;
            case SYSTEM_USAGE:
                newDevice = new SystemUsageDevice();
                break;
            default:
                throw std::runtime_error(fmt::format("Device {} has unknown protocol {}.",
                                                     deviceDefinition.name,
                                                     deviceDefinition.protocol));
            }
            newDevice->open(deviceDefinition.name);
            deviceList.emplace_back(newDevice);
        }

        isRunning = true;
    }
}

void DeviceManager::update() {
    if (timer->check()) {
        for (auto& device : deviceList) {
            device->update();
        }
    }
}

void DeviceManager::stop() {
    if (isRunning) {
        for (auto& device : deviceList) {
            record_log_t::createLog(
                fmt::format("Stopped device (address={})", device->getAddress()));
            device->close();
            delete device;
        }
        deviceList.clear();
        portList.clear();
        SM::getLogger()->alert("Stopped device manager!");
        record_log_t::createLog(fmt::format("Stopped devices."));
        isRunning = false;
    }
}

std::map<int, std::string> DeviceManager::getProtocolMap() {
    std::map<int, std::string> map;
    map[NMEA_0183] = Nmea0183Device::name();
    map[SYSTEM_USAGE] = SystemUsageDevice::name();
    return map;
}
