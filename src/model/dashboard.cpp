#include "dashboard.h"
#include "services/service_manager.h"
#include <fmt/format.h>
#include <utility>

record_dashboard_t record_dashboard_t::createDashboard(std::string name) {
    auto dashboard = SM::getDatabase()->createDashboard(
        {-1, record_system_t::getActiveSystem().id, std::move(name), "{}"});
    SM::getLogger()->info(fmt::format("Created new dashboard: [id={}, name={}]",
                                      dashboard.id,
                                      dashboard.name),
                          true);
    return dashboard;
}

record_dashboard_t record_dashboard_t::getDashboard(int id) {
    return SM::getDatabase()->getDashboard(id);
}

std::vector<record_dashboard_t> record_dashboard_t::getDashboards() {
    return SM::getDatabase()->getDashboards(record_system_t::getActiveSystem().id);
}

record_dashboard_t record_dashboard_t::updateDashboard(record_dashboard_t recordToUpdate) {
    SM::getDatabase()->updateDashboard(recordToUpdate);
    SM::getLogger()->info(fmt::format("Updated dashboard: [id={}, name={}]",
                                      recordToUpdate.id,
                                      recordToUpdate.name),
                          true);
    return recordToUpdate;
}

void record_dashboard_t::deleteDashboard(const record_dashboard_t& recordToDelete) {
    deleteDashboard(recordToDelete.id);
}

void record_dashboard_t::deleteDashboard(int id) {
    SM::getDatabase()->deleteById("dashboards", id);
    SM::getLogger()->info(fmt::format("Deleted dashboard: [id={}]", id), true);
}