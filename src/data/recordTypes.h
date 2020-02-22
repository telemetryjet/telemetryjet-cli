#ifndef TELEMETRYSERVER_RECORDTYPES_H
#define TELEMETRYSERVER_RECORDTYPES_H

#include <string>

struct record_dashboard_t {
    int id;
    std::string name;
    std::string jsonDefinition;
};

struct record_data_t {
    int id;
    int packet_id;
    long timestamp;
    std::string key;
    std::string value;
};

struct record_debug_log_t {
    int id;
    long timestamp;
    std::string message;
};

#endif //TELEMETRYSERVER_RECORDTYPES_H
