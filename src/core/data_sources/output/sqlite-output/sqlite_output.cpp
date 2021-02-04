#include "sqlite_output.h"

SqliteOutputDataSource::SqliteOutputDataSource(const json& definition)
    : DataSource(definition) {

    if (options.contains("db_directory")) {
        dbDirectory = options["db_directory"];
    } else {
        SM::getLogger()->info(
            fmt::format("No database directory provided. Using default '{}'", dbDirectory));
    }

    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    std::string databasePath = fmt::format("file:{}/database.db", dbDirectory);
    ServiceManager::getLogger()->info(fmt::format("SQLite database path: {}", databasePath));

    db = new SQLite::Database(databasePath,
                              SQLite::OPEN_URI | SQLite::OPEN_READWRITE
                                  | SQLite::OPEN_CREATE);  // NOLINT(hicpp-signed-bitwise)

    db->exec(
        "create table if not exists data (id integer primary key, key text not null, timestamp "
        "text not null, data_type integer not null, value text not null)");
    inputEnabled = false;
}

SqliteOutputDataSource::~SqliteOutputDataSource() {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope
    delete db;
}

void SqliteOutputDataSource::update() {
    if (!in.empty()) {
        const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

        for (auto& dp : in) {
            try {
                SQLite::Statement insertStatement(*db, "insert into data values (null,?,?,?,?)");
                insertStatement.bind(1, dp->key);
                insertStatement.bind(2, static_cast<long long>(dp->timestamp));
                insertStatement.bind(3, static_cast<int>(dp->type));
                insertStatement.bind(4, dp->toString());
                insertStatement.exec();
            } catch (std::exception& e) {
                throw std::runtime_error(
                    fmt::format("Error adding datapoint to sqlite DB: {}", e.what()));
            }
        }
    }
}