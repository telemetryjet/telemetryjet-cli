#include "sqlite_database.h"
#include <fmt/format.h>

std::vector<record_server_t> SqliteDatabase::getAllServers() {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    std::vector<record_server_t> servers;
    try {
        SQLite::Statement query(*db, "select * from servers");

        while (query.executeStep()) {
            servers.push_back({query.getColumn(0),
                                  query.getColumn(1),
                                  query.getColumn(2),
                                  query.getColumn(3)});
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getAllServers: {}", e.what()));
    }
    return servers;
}

record_server_t SqliteDatabase::getServer(const std::string& id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from servers where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()) {
        return {query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2),
                query.getColumn(3)};
    } else {
        throwError(fmt::format("Error in getServer: Server with id = {} not found.", id));
    }
}

bool SqliteDatabase::serverExists(const std::string& id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from servers where id=?");
    query.bind(1, id);
    query.executeStep();
    return query.hasRow();
}

record_server_t SqliteDatabase::createServer(record_server_t server) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into servers values (?,?,?,?)");
        insertStatement.bind(1, server.alias);
        insertStatement.bind(2, server.host);
        insertStatement.bind(3, server.username);
        insertStatement.bind(4, server.password);
        insertStatement.exec();
        return server;
    } catch (std::exception& e) {
        throwError(fmt::format("Error in createServer: {}", e.what()));
    }
}

void SqliteDatabase::deleteServer(const std::string& id) {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement deleteStatement(*db, fmt::format("delete from servers where id=?"));
        deleteStatement.bind(1, id);
        deleteStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in deleteServer: {}", e.what()));
    }
}

void SqliteDatabase::deleteAllServers() {
    const std::lock_guard<std::mutex> lock(databaseMutex);  // Acquire database lock for this scope

    try {
        SQLite::Statement deleteStatement(*db, fmt::format("delete from servers"));
        deleteStatement.exec();
    } catch (std::exception& e) {
        throwError(fmt::format("Error in deleteAllServers: {}", e.what()));
    }
}