#include "SqliteDatabase.h"

record_dashboard_t SqliteDatabase::createDashboard(std::string name, std::string jsonDefinition) {
    record_dashboard_t dash = {-1, name, jsonDefinition};
    try {
        SQLite::Statement insertStatement(db, "insert into dashboards values (null,?,?)");
        insertStatement.bind(1, dash.name);
        insertStatement.bind(2, dash.jsonDefinition);
        insertStatement.exec();
        dash.id = db.getLastInsertRowid();
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
        return dash;
    }
    return dash;
}
record_dashboard_t SqliteDatabase::updateDashboard(record_dashboard_t input) {
    try {
        SQLite::Statement updateStatement(db, "update dashboards set name=?, jsonDefinition=? where id=?");
        updateStatement.bind(1, input.name);
        updateStatement.bind(2, input.jsonDefinition);
        updateStatement.bind(3, input.id);
        updateStatement.exec();
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return input;
}

record_dashboard_t SqliteDatabase::getDashboard(int id) {
    SQLite::Statement query(db, "select * from dashboards where id=?");
    query.bind(1, id);
    query.executeStep();
    if (query.hasRow()){
        return {
                query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2)
        };
    } else {
        throw std::runtime_error("Dashboard object not found");
    }
}

void SqliteDatabase::deleteDashboard(int id) {
    try {
        SQLite::Statement updateStatement(db, "delete from dashboards where id=?");
        updateStatement.bind(1, id);
        updateStatement.exec();
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

std::vector<record_dashboard_t> SqliteDatabase::getDashboards() {
    std::vector<record_dashboard_t> dashboardList;
    try {
        SQLite::Statement query(db, "select * from dashboards");
        while (query.executeStep()) {
            dashboardList.push_back({
                                            query.getColumn(0),
                                            query.getColumn(1),
                                            query.getColumn(2)
                                    });
        }
    } catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return dashboardList;
}