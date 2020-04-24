#include "device.h"
#include "services/service_manager.h"
#include <utility>

record_device_t record_device_t::createDevice(std::string name, int protocol) {
    return SM::getDatabase()->createDevice(
        {-1, record_system_t::getActiveSystem().id, std::move(name), protocol});
}

record_device_t record_device_t::getDevice(int id) {
    return SM::getDatabase()->getDevice(id);
}

std::vector<record_device_t> record_device_t::getDevices() {
    return SM::getDatabase()->getDevices(record_system_t::getActiveSystem().id);
}

record_device_t record_device_t::updateDevice(record_device_t recordToUpdate) {
    SM::getDatabase()->updateDevice(recordToUpdate);
    return recordToUpdate;
}

void record_device_t::deleteDevice(const record_device_t& recordToDelete) {
    deleteDevice(recordToDelete.id);
}

void record_device_t::deleteDevice(int id) {
    SM::getDatabase()->deleteById("devices", id);
}