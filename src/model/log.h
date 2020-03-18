#ifndef TELEMETRYSERVER_LOG_H
#define TELEMETRYSERVER_LOG_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A Log.
 */
struct record_log_t {
    int id;
    int system_id;
    std::string message;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("message", message);
        return pt;
    }

    // Basic Create, Read, Update, Delete functions
    static             record_log_t  createLog(std::string message);
    static             record_log_t  getLog(int id);
    static std::vector<record_log_t> getLogs();
    static                     void  updateLog(record_log_t recordToUpdate);
    static                     void  deleteLog(const record_log_t& recordToDelete);
    static                     void  deleteLog(int id);
    static                     void  clearLogs();
};

#endif //TELEMETRYSERVER_LOG_H
