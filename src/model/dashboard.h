#ifndef TELEMETRYSERVER_DASHBOARD_H
#define TELEMETRYSERVER_DASHBOARD_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A Dashboard.
 */
struct record_dashboard_t {
    int id;
    int system_id;
    std::string name;
    std::string jsonDefinition;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("id", id);
        pt.add("system_id", system_id);
        pt.add("name", name);
        pt.add("jsonDefinition", jsonDefinition);
        return pt;
    }

    // Basic Create, Read, Update, Delete functions
    static             record_dashboard_t  createDashboard(std::string name);
    static             record_dashboard_t  getDashboard(int id);
    static std::vector<record_dashboard_t> getDashboards();
    static                           void  updateDashboard(record_dashboard_t recordToUpdate);
    static                           void  deleteDashboard(const record_dashboard_t& recordToDelete);
    static                           void  deleteDashboard(int id);
};

#endif //TELEMETRYSERVER_DASHBOARD_H
