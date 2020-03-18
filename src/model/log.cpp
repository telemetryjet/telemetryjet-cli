#include "log.h"
#include "services/service_manager.h"
#include <fmt/format.h>
#include <utility>
#include "utility/time_utils.h"

record_log_t record_log_t::createLog(std::string message) {
    std::string timestamp = fmt::format("{}",getTimestamp());
    return SM::getDatabase()->createLog({ -1, record_system_t::getActiveSystem().id, timestamp, "log", message });
}

record_log_t record_log_t::getLog(int id) {
    return SM::getDatabase()->getLog(id);
}

std::vector<record_log_t> record_log_t::getLogs() {
    return SM::getDatabase()->getLogs(record_system_t::getActiveSystem().id);
}

record_log_t record_log_t::updateLog(record_log_t recordToUpdate) {
    SM::getDatabase()->updateLog(std::move(recordToUpdate));
    return recordToUpdate;
}

void record_log_t::deleteLog(const record_log_t& recordToDelete) {
    SM::getDatabase()->deleteById("logs",recordToDelete.id);
}

void record_log_t::deleteLog(int id) {
    SM::getDatabase()->deleteById("logs",id);
}

void record_log_t::clearLogs() {
    SM::getDatabase()->deleteAll("logs");
}