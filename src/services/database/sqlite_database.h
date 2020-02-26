#ifndef TELEMETRYSERVER_SQLITE_DATABASE_H
#define TELEMETRYSERVER_SQLITE_DATABASE_H

#include <SQLiteCpp/Database.h>
#include "database.h"

class SqliteDatabase : public Database {
private:
    SQLite::Database *db;
    bool initialized = false;
public:
    SqliteDatabase();
    ~SqliteDatabase() override;
};


#endif //TELEMETRYSERVER_SQLITE_DATABASE_H
