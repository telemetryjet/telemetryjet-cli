#include <services/ServiceManager.h>
#include "SqliteDatabase.h"

record_data_t SqliteDatabase::createDataRow(long timestamp, std::string key, std::string value) {
    std::cout << "Inserting data row { " << timestamp << ", " << key << ", " << value << " }\n";
    record_data_t record = {-1, packet_id, timestamp, key, value};
    try {
        SQLite::Statement insertStatement(db, "insert into data values (null,?,?,?,?)");
        insertStatement.bind(1, record.packet_id);
        insertStatement.bind(2, record.timestamp);
        insertStatement.bind(3, record.key);
        insertStatement.bind(4, record.value);
        insertStatement.exec();
        record.id = db.getLastInsertRowid();
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return record;
}

std::vector<record_data_t> SqliteDatabase::getDataRows() {
    std::vector<record_data_t> dataList;
    try {
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
    }
    catch (std::exception& e)
    {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return dataList;
}

void SqliteDatabase::startPacket() {
    //ServiceManager::getLogger().info("Starting data packet!");
    std::cout << "Starting data packet " << packet_id << "\n";
}

void SqliteDatabase::endPacket() {
    std::cout << "Finishing data packet " << packet_id << "\n";
    packet_id++;
}
