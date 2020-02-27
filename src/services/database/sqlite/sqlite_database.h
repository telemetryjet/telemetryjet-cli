#ifndef TELEMETRYSERVER_SQLITE_DATABASE_H
#define TELEMETRYSERVER_SQLITE_DATABASE_H

#include <SQLiteCpp/Database.h>
#include <vector>
#include "services/database/database.h"

class SqliteDatabase : public Database {
private:
    SQLite::Database *db;
    bool initialized = false;
public:
    SqliteDatabase();
    ~SqliteDatabase() override;

    // Systems
    std::vector<record_system_t> getSystems() override;
                record_system_t  getSystem(int id) override;
                record_system_t  createSystem(record_system_t system) override;
                           void  updateSystem(record_system_t system) override;
                           void  deleteSystem(int id) override;
};

#endif //TELEMETRYSERVER_SQLITE_DATABASE_H
