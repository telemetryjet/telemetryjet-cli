//
// Created by Chris Dalke on 1/29/20.
//

#include "database.h"
#include <sqlite3.h>
#include <string>
#include <boost/filesystem.hpp>
#include <iostream>
#include <SQLiteCpp/Database.h>
namespace fs = boost::filesystem;

Database::Database() : db("test.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    std::cout << "Initialized local database at file [" << db.getFilename() << "]\n";
    std::cout << "Current dir: " << fs::current_path().generic_string() << "\n";
    db.exec("create table if not exists dashboards (id integer primary key, name text not null, jsonDefinition text not null)");
    db.exec("create table if not exists data (id integer primary key, packet_id integer, timestamp integer not null, key text not null, value text not null)");

    // Check max packet id
    SQLite::Statement query(db, "select ifnull(max(packet_id),0) from data;");
    query.executeStep();
    packet_id = ((int)query.getColumn(0)) + 1;
    std::cout << "Starting with data packet ID " << packet_id << "\n";
}

void Database::startPacket() {
    std::cout << "Starting data packet " << packet_id << "\n";
}
void Database::endPacket() {
    std::cout << "Finishing data packet " << packet_id << "\n";
    packet_id++;
}

record_data_t Database::createDataRow(long timestamp, std::string key, std::string value) {
    std::cout << "Inserting data row { " << timestamp << ", " << key << ", " << value << " }\n";
    record_data_t record = {-1, packet_id, timestamp, key, value};
    SQLite::Statement insertStatement(db, "insert into data values (null,?,?,?,?)");
    insertStatement.bind(1, record.packet_id);
    insertStatement.bind(2, record.timestamp);
    insertStatement.bind(3, record.key);
    insertStatement.bind(4, record.value);
    insertStatement.exec();
    record.id = db.getLastInsertRowid();
    return record;
}

std::vector<record_data_t> Database::getDataRows() {
    std::vector<record_data_t> dataList;
    SQLite::Statement query(db, "select * from data");
    while (query.executeStep()) {
        dataList.push_back({
                           query.getColumn(0),
                           query.getColumn(1),
                           query.getColumn(2),
                           query.getColumn(3),
                           query.getColumn(4)
                   });
    }
    return dataList;
}

/*
record_data_t Database::getDataRow(int id) {

}


record_dashboard_t Database::createDashboard(std::string name, std::string jsonDefinition) {

}

record_dashboard_t Database::updateDashboard(int id, std::string name, std::string jsonDefinition) {

}

record_dashboard_t Database::getDashboard(int id){

}

std::vector<record_dashboard_t> Database::getDashboards(){

}
 */