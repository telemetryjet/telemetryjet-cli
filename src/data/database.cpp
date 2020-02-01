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

Database::Database(const DataCache &cache) : cache(cache), db("test.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    std::cout << "Initialized local database at file [" << db.getFilename() << "]\n";
    std::cout << "Current dir: " << fs::current_path().generic_string() << "\n";
    db.exec("create table if not exists dashboards (id integer primary key, name text not null, jsonDefinition text not null)");
    db.exec("create table if not exists data (id integer primary key, timestamp integer not null, key text not null, value text not null)");

}

Database::~Database() {
}

record_data_t Database::createDataRow(int timestamp, std::string key, std::string value) {
    record_data_t record = {-1, timestamp, key, value};
    SQLite::Statement insertStatement(db, "insert into data values (?,?,?)");
    insertStatement.bind(0, record.timestamp);
    insertStatement.bind(1, record.key);
    insertStatement.bind(2, record.value);
    insertStatement.exec();
    record.id = db.getLastInsertRowid();
    return record;
}

record_data_t Database::updateDataRow(int id, record_data_t rowData) {

}

record_data_t Database::getDataRow(int id) {

}

std::vector<record_data_t> Database::getDataRows() {
    std::vector<record_data_t> dataList;
    SQLite::Statement query(db, "select * from data");
    while (query.executeStep()) {
        dataList.push_back({
                query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2),
                query.getColumn(2)
        });
    }
    return dataList;
}

record_dashboard_t Database::createDashboard(std::string name, std::string jsonDefinition) {

}

record_dashboard_t Database::updateDashboard(int id, std::string name, std::string jsonDefinition) {

}

record_dashboard_t Database::getDashboard(int id){

}

std::vector<record_dashboard_t> Database::getDashboards(){

}