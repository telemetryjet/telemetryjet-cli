#include "SqliteDatabase.h"
#include <boost/filesystem.hpp>
#include <services/ServiceManager.h>

namespace fs = boost::filesystem;

SqliteDatabase::SqliteDatabase(std::string filename) : db(filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    //ServiceManager::getLogger().info("Initialized local database.");
    std::cout <<  "Initialized local database with filename [" << db.getFilename() << "]\n";
    std::cout << "Current dir: " << fs::current_path().generic_string() << "\n";
    db.exec("create table if not exists dashboards (id integer primary key, name text not null, jsonDefinition text not null)");
    db.exec("create table if not exists data (id integer primary key, packet_id integer, timestamp integer not null, key text not null, value text not null)");
    db.exec("create table if not exists debug_logs (id integer primary key, timestamp integer not null, message text not null)");

    // Check max packet id
    SQLite::Statement query(db, "select ifnull(max(packet_id),0) from data;");
    query.executeStep();
    packet_id = ((int)query.getColumn(0)) + 1;
    //std::cout << "Starting with data packet ID " << packet_id << "\n";
}
