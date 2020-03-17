#include <fmt/format.h>
#include "sqlite_database.h"

std::vector<record_config_item_t> SqliteDatabase::getConfigItems()  {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    std::vector<record_config_item_t> configItems;
    try {
        SQLite::Statement query(*db, "select * from config_items");
        while (query.executeStep()) {
            configItems.push_back({
                                      query.getColumn(0),
                                      query.getColumn(1)
                              });
        }
    } catch (std::exception& e) {
        throwError(fmt::format("Error in getConfigItems: {}", e.what()));
    }
    return configItems;
}

record_config_item_t SqliteDatabase::getConfigItem(std::string key)  {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    SQLite::Statement query(*db, "select * from config_items where key=?");
    query.bind(1, key);
    query.executeStep();
    if (query.hasRow()){
        return {
                query.getColumn(0),
                query.getColumn(1)
        };
    } else {
        throwError(fmt::format("Error in getConfigItem: Config Item with key = {} not found.",key));
    }
}

record_config_item_t SqliteDatabase::createConfigItem(record_config_item_t configItem)  {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement insertStatement(*db, "insert into config_items values (?,?)");
        insertStatement.bind(1, configItem.key);
        insertStatement.bind(2, configItem.value);
        insertStatement.exec();
        return configItem;
    } catch (std::exception &e) {
        throwError(fmt::format("Error in createConfigItem: {}", e.what()));
    }
}

void SqliteDatabase::updateConfigItem(record_config_item_t configItem)  {
    const std::lock_guard<std::mutex> lock(databaseMutex); // Acquire database lock for this scope

    try {
        SQLite::Statement updateStatement(*db, "update config_items set value=? where key=?");
        updateStatement.bind(1, configItem.value);
        updateStatement.bind(2, configItem.key);
        updateStatement.exec();
    } catch (std::exception &e) {
        throwError(fmt::format("Error in updateConfigItem: {}", e.what()));
    }
}
