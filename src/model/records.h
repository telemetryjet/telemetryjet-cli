#ifndef TELEMETRYSERVER_RECORDS_H
#define TELEMETRYSERVER_RECORDS_H

#include <string>

struct record_system_t {
    int id;
    std::string name;
};

/*
struct record_dashboard_t {
    int id;
    int system_id;
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
*/

#endif //TELEMETRYSERVER_RECORDS_H
