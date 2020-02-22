#include "SqliteDatabase.h"

record_debug_log_t SqliteDatabase::createDebugLog(long timestamp, std::string message) {
    record_debug_log_t record = {-1, timestamp, message};
    try {
        SQLite::Statement insertStatement(db, "insert into debug_logs values (null,?,?)");
        insertStatement.bind(1, record.timestamp);
        insertStatement.bind(2, record.message);
        insertStatement.exec();
        record.id = db.getLastInsertRowid();
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return record;
}

std::vector<record_debug_log_t> SqliteDatabase::getDebugLogs() {
    std::vector<record_debug_log_t> debugLogList;
    try {
        SQLite::Statement query(db, "select * from debug_logs");
        while (query.executeStep()) {
            debugLogList.push_back({
                                           query.getColumn(0),
                                           query.getColumn(1),
                                           query.getColumn(2)
                                   });
        }
    } catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return debugLogList;
}