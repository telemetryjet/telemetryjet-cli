#ifndef TELEMETRYJETCLI_SQLITE_OUTPUT_H
#define TELEMETRYJETCLI_SQLITE_OUTPUT_H

#include "core/data_source.h"
#include <SQLiteCpp/Database.h>

class SqliteOutputDataSource : public DataSource {
private:
    SQLite::Database* db;
    std::mutex databaseMutex;
    std::string dbDirectory = ".";
public:
    SqliteOutputDataSource(const json& definition);
    ~SqliteOutputDataSource();
    void update() override;
};

#endif
