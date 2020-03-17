#include <fmt/format.h>
#include "sqlite_database.h"

std::vector<record_dashboard_t> SqliteDatabase::getDashboards(int system_id) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    std::vector<record_dashboard_t> dashboards;
    try {
        SQLite::Statement query(*db, fmt::format("select * from dashboards where system_id={}", system_id));
        while (query.executeStep()) {
            dashboards.push_back({
                                      query.getColumn(0),
                                      query.getColumn(1),
                                      query.getColumn(2),
                                      query.getColumn(3)
                              });
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getDashboards: {}", e.what()));
    }
    return dashboards;
}

record_dashboard_t SqliteDatabase::getDashboard(int id) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from dashboards where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()){
        return {
                query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2),
                query.getColumn(3)
        };
    } else {
        throwError(fmt::format("Error in getDashboard: Dashboard with id = {} not found.",id));
    }
}

record_dashboard_t SqliteDatabase::createDashboard(record_dashboard_t dashboard) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into dashboards values (null,?,?,?)");
        insertStatement.bind(1, dashboard.system_id);
        insertStatement.bind(2, dashboard.name);
        insertStatement.bind(3, dashboard.jsonDefinition);
        insertStatement.exec();
        dashboard.id = db->getLastInsertRowid();
        return dashboard;
    } catch (std::exception &e) {
        throwError(fmt::format("Error in createDashboard: {}", e.what()));
    }
}

void SqliteDatabase::updateDashboard(record_dashboard_t dashboard) {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement updateStatement(*db, "update dashboards set system_id=?, name=?, jsonDefinition=? where id=?");
        updateStatement.bind(1, dashboard.system_id);
        updateStatement.bind(2, dashboard.name);
        updateStatement.bind(3, dashboard.jsonDefinition);
        updateStatement.bind(4, dashboard.id);
        updateStatement.exec();
    } catch (std::exception &e) {
        throwError(fmt::format("Error in updateDashboard: {}", e.what()));
    }
}