#include "dashboard.h"
#include "services/service_manager.h"
#include <fmt/format.h>
#include <utility>

record_dashboard_t record_dashboard_t::createDashboard(std::string name) {
    return SM::getDatabase()->createDashboard(
        {-1, record_system_t::getActiveSystem().id, std::move(name), "{}"});
}

record_dashboard_t record_dashboard_t::getDashboard(int id) {
    return SM::getDatabase()->getDashboard(id);
}

std::vector<record_dashboard_t> record_dashboard_t::getDashboards() {
    return SM::getDatabase()->getDashboards(record_system_t::getActiveSystem().id);
}

record_dashboard_t record_dashboard_t::updateDashboard(record_dashboard_t recordToUpdate) {
    SM::getDatabase()->updateDashboard(recordToUpdate);
    return recordToUpdate;
}

void record_dashboard_t::deleteDashboard(const record_dashboard_t& recordToDelete) {
    deleteDashboard(recordToDelete.id);
}

void record_dashboard_t::deleteDashboard(int id) {
    SM::getDatabase()->deleteById("dashboards", id);
}