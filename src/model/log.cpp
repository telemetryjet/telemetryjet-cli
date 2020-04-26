#include "log.h"
#include "services/api/streaming/streaming_server.h"
#include "services/service_manager.h"
#include "utility/time_utils.h"
#include <fmt/format.h>
#include <utility>

record_log_t record_log_t::createLog(std::string message, std::string level) {
    std::string timestamp = fmt::format("{}", getTimestamp());
    record_log_t log({-1,
                      record_system_t::getActiveSystem().id,
                      timestamp,
                      std::move(level),
                      std::move(message)});
    SM::getStreamingServer()->sendMessageToAll(StreamingServer::OutgoingMessageType::NEW_LOG,
                                               log.toPropertyTree());
    return SM::getDatabase()->createLog(log);
}

record_log_t record_log_t::getLog(int id) {
    return SM::getDatabase()->getLog(id);
}

std::vector<record_log_t> record_log_t::getLogs() {
    return SM::getDatabase()->getRecentLogs(record_system_t::getActiveSystem().id, 100);
}

record_log_t record_log_t::updateLog(record_log_t recordToUpdate) {
    SM::getDatabase()->updateLog(std::move(recordToUpdate));
    return recordToUpdate;
}

void record_log_t::deleteLog(const record_log_t& recordToDelete) {
    SM::getDatabase()->deleteById("logs", recordToDelete.id);
}

void record_log_t::deleteLog(int id) {
    SM::getDatabase()->deleteById("logs", id);
}

void record_log_t::clearLogs() {
    SM::getDatabase()->deleteAll("logs");
}