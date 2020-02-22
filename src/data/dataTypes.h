#ifndef TELEMETRYSERVER_DATATYPES_H
#define TELEMETRYSERVER_DATATYPES_H

#include <string>

struct data_key_t {
    long lastUpdated;
    long expireAge;
    std::string key;
    float value;
    float max_value;
    float min_value;
    float avg_value;
    long avg_value_quantity;
    bool expired;
};

#endif //TELEMETRYSERVER_DATATYPES_H
