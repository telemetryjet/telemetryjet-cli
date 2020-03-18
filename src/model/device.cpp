#include "device.h"
#include "services/service_manager.h"
#include <fmt/format.h>
#include <utility>

record_device_t record_device_t::createDevice(std::string name) {
    return SM::getDatabase()->createDevice({
        -1,
        record_system_t::getActiveSystem().id,
        std::move(name)
    });
}

record_device_t record_device_t::getDevice(int id) {
    return SM::getDatabase()->getDevice(id);

}

std::vector<record_device_t> record_device_t::getDevices() {
    return SM::getDatabase()->getDevices(record_system_t::getActiveSystem().id);

}

record_device_t record_device_t::updateDevice(record_device_t recordToUpdate) {
    SM::getDatabase()->updateDevice(std::move(recordToUpdate));
    return recordToUpdate;

}

void record_device_t::deleteDevice(const record_device_t& recordToDelete) {
    SM::getDatabase()->deleteById("devices",recordToDelete.id);

}

void record_device_t::deleteDevice(int id) {
    SM::getDatabase()->deleteAll("devices");
}