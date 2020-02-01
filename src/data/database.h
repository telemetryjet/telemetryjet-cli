//
// Created by Chris Dalke on 1/29/20.
//

#ifndef TELEMETRYSERVER_DATABASE_H
#define TELEMETRYSERVER_DATABASE_H

#include <sqlite3.h>
#include <SQLiteCpp/Database.h>
#include "dataCache.h"

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

class Database {
private:
    int packet_id;
    float test{};
    bool initialized;
    SQLite::Database db;
public:
    Database();
    record_data_t createDataRow(long timestamp, std::string key, std::string value);
    //record_data_t getDataRow(int id);
    std::vector<record_data_t> getDataRows();
    //record_dashboard_t createDashboard(std::string name, std::string jsonDefinition);
    //record_dashboard_t updateDashboard(int id, std::string name, std::string jsonDefinition);
    //record_dashboard_t getDashboard(int id);
    //std::vector<record_dashboard_t> getDashboards();
    void startPacket();
    void endPacket();
};

#endif //TELEMETRYSERVER_DATABASE_H
