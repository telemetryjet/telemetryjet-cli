#include "rest_api_server.h"
#include "constants.h"
#include "utility/json_utils.h"
#include <boost/property_tree/exceptions.hpp>
#include <fmt/format.h>
#include <functional>
#include <model/records.h>
#include <services/service_manager.h>

#define REQUEST_RESPONSE_PARAMS                                                                    \
    const std::shared_ptr<HttpServer::Response>&response,                                          \
        const std::shared_ptr<HttpServer::Request>&request

using namespace boost::property_tree;
using namespace SimpleWeb;

// Helper function for parsing an int ID from the request path
int getIntPathParam(const std::shared_ptr<HttpServer::Request>& request, int position) {
    std::istringstream idStream(request->path_match[position].str());
    int id;
    idStream >> id;
    if (idStream.fail()) {
        throw std::runtime_error("Failed to parse integer from request path.");
    } else {
        return id;
    }
}

void genericOptionsResponse(REQUEST_RESPONSE_PARAMS) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Access-Control-Allow-Headers", "*");
    commonHeader.emplace("Access-Control-Allow-Methods", "POST, PUT, GET, OPTIONS, DELETE");
    commonHeader.emplace("Content-Type", "application/json");
    response->write(StatusCode::success_ok, commonHeader);
}

void sendSuccessResponse(const std::shared_ptr<HttpServer::Response>& response,
                         std::string content) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Access-Control-Allow-Headers", "*");
    commonHeader.emplace("Access-Control-Allow-Methods", "POST, PUT, GET, OPTIONS, DELETE");
    commonHeader.emplace("Content-Type", "application/json");
    commonHeader.emplace("Content-Length", fmt::format("{}", content.length()));
    // SM::getLogger()->info(fmt::format("{}",content));
    // SM::getLogger()->info(fmt::format("Content-Length: {}",content.length()));
    response->write(StatusCode::success_ok, content, commonHeader);
}

void sendFailureResponse(const std::shared_ptr<HttpServer::Response>& response,
                         std::string content) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Access-Control-Allow-Headers", "*");
    commonHeader.emplace("Access-Control-Allow-Methods", "POST, PUT, GET, OPTIONS, DELETE");
    commonHeader.emplace("Content-Type", "application/json");
    commonHeader.emplace("Content-Length", fmt::format("{}", content.length()));
    // SM::getLogger()->info(fmt::format("{}",content));
    // SM::getLogger()->info(fmt::format("Content-Length: {}",content.length()));
    response->write(StatusCode::client_error_bad_request, content, commonHeader);
}

// Helper functions used to handle requests
void handleStatus(REQUEST_RESPONSE_PARAMS) {
    sendSuccessResponse(response, "ONLINE");
}

void handleSystemState(REQUEST_RESPONSE_PARAMS) {
    if (record_system_t::isSystemRunning()) {
        sendSuccessResponse(response, "{\"enabled\" : true }");
    } else {
        sendSuccessResponse(response, "{\"enabled\" : false }");
    }
}

void handleSystemStateChange(REQUEST_RESPONSE_PARAMS) {
    boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());

    bool enabled = pt.get<bool>("enabled");
    if (enabled) {
        record_system_t::startSystem();
    } else {
        record_system_t::stopSystem();
    }
    handleSystemState(response, request);
}

void handleSetActiveSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        int id = pt.get<int>("id");
        record_system_t::setActiveSystem(id);
        sendSuccessResponse(response, fmt::format("{{\"id\" : {} }}", id));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetActiveSystem(REQUEST_RESPONSE_PARAMS) {
    record_system_t activeSystem = record_system_t::getActiveSystem();
    sendSuccessResponse(response, fmt::format("{{\"id\" : {} }}", activeSystem.id));
}

void handleGetSystems(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        const std::vector<record_system_t>& systems = record_system_t::getSystems();
        if (systems.empty()) {
            sendSuccessResponse(response, "{\"systems\" : []}");
            return;
        }

        for (const record_system_t& system : systems) {
            list.push_back(std::make_pair("", system.toPropertyTree()));
        }
        pt.add_child("systems", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        record_system_t system = record_system_t::getSystem(id);
        std::string jsonString = propertyTreeToString(system.toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleCreateSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        SM::getLogger()->info(
            fmt::format("Creating system from json [{}]", request->content.string()));
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_system_t record = record_system_t::createSystem(pt.get<std::string>("name"));
        boost::property_tree::ptree newSystem = record.toPropertyTree();
        SM::getStreamingServer()->sendMessageToAll(StreamingServer::OutgoingMessageType::NEW_SYSTEM,
                                                   newSystem);
        sendSuccessResponse(response, propertyTreeToString(newSystem));
    } catch (std::exception& error) {
        SM::getLogger()->error(error.what());
        sendFailureResponse(response, error.what());
    }
}

void handleUpdateSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_system_t record = record_system_t::updateSystem({id, pt.get<std::string>("name")});
        boost::property_tree::ptree newSystem = record.toPropertyTree();
        SM::getStreamingServer()
            ->sendMessageToAll(StreamingServer::OutgoingMessageType::UPDATE_SYSTEM, newSystem);
        sendSuccessResponse(response, propertyTreeToString(newSystem));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleDeleteSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        record_system_t::deleteSystem(getIntPathParam(request, 1));
        sendSuccessResponse(response, "");
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetDevices(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        const std::vector<record_device_t>& devices = record_device_t::getDevices();
        if (devices.empty()) {
            sendSuccessResponse(response, "{\"devices\" : []}");
            return;
        }

        for (const record_device_t& device : devices) {
            list.push_back(std::make_pair("", device.toPropertyTree()));
        }
        pt.add_child("devices", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleCreateDevice(REQUEST_RESPONSE_PARAMS) {
    try {
        SM::getLogger()->info(
            fmt::format("Creating device from json [{}]", request->content.string()));
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_device_t record
            = record_device_t::createDevice(pt.get<std::string>("name"), pt.get<int>("protocol"));
        boost::property_tree::ptree newDevice = record.toPropertyTree();
        SM::getStreamingServer()->sendMessageToAll(StreamingServer::OutgoingMessageType::NEW_DEVICE,
                                                   newDevice);
        sendSuccessResponse(response, propertyTreeToString(newDevice));
    } catch (std::exception& error) {
        SM::getLogger()->error(error.what());
        sendFailureResponse(response, error.what());
    }
}

void handleGetDevice(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        std::string jsonString
            = propertyTreeToString(record_device_t::getDevice(id).toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleUpdateDevice(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_device_t record = record_device_t::updateDevice(
            {id, pt.get<int>("system_id"), pt.get<std::string>("name"), pt.get<int>("protocol")});
        boost::property_tree::ptree newDevice = record.toPropertyTree();
        SM::getStreamingServer()
            ->sendMessageToAll(StreamingServer::OutgoingMessageType::UPDATE_DEVICE, newDevice);
        sendSuccessResponse(response, propertyTreeToString(newDevice));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleDeleteDevice(REQUEST_RESPONSE_PARAMS) {
    try {
        record_device_t::deleteDevice(getIntPathParam(request, 1));
        sendSuccessResponse(response, "");
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetProtocols(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        for (auto const& protocol : DeviceManager::getProtocolMap()) {
            boost::property_tree::ptree node;
            node.put("id", protocol.first);
            node.put("name", protocol.second);
            list.push_back(std::make_pair("", node));
        }
        pt.add_child("protocols", list);
        sendSuccessResponse(response, propertyTreeToString(pt));

    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetLogs(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        int limit = 100;
        int beforeId = -1;
        auto query_fields = request->parse_query_string();
        for (auto& field : query_fields) {
            if (field.first == LIMIT_QUERY_PARAM) {
                limit = std::stoi(field.second);
            } else if (field.first == BEFORE_ID_QUERY_PARAM) {
                beforeId = std::stoi(field.second);
            }
        }

        const std::vector<record_log_t>& logs = record_log_t::getLogs(limit, beforeId);
        if (logs.empty()) {
            sendSuccessResponse(response, "{\"logs\" : []}");
            return;
        }

        for (const record_log_t& log : logs) {
            list.push_back(std::make_pair("", log.toPropertyTree()));
        }
        pt.add_child("logs", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleDeleteAllLogs(REQUEST_RESPONSE_PARAMS) {
    try {
        record_log_t::clearLogs();
        sendSuccessResponse(response, "");
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetLog(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        std::string jsonString = propertyTreeToString(record_log_t::getLog(id).toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleDeleteLog(REQUEST_RESPONSE_PARAMS) {
    try {
        record_log_t::deleteLog(getIntPathParam(request, 1));
        sendSuccessResponse(response, "");
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetDashboards(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        const std::vector<record_dashboard_t>& dashboards = record_dashboard_t::getDashboards();
        if (dashboards.empty()) {
            sendSuccessResponse(response, "{\"dashboards\" : []}");
            return;
        }

        for (const record_dashboard_t& dashboard : dashboards) {
            list.push_back(std::make_pair("", dashboard.toPropertyTree()));
        }
        pt.add_child("dashboards", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleCreateDashboard(REQUEST_RESPONSE_PARAMS) {
    try {
        SM::getLogger()->info(
            fmt::format("Creating dashboard from json [{}]", request->content.string()));
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_dashboard_t record
            = record_dashboard_t::createDashboard(pt.get<std::string>("name"));
        boost::property_tree::ptree newDashboard = record.toPropertyTree();
        SM::getStreamingServer()
            ->sendMessageToAll(StreamingServer::OutgoingMessageType::NEW_DASHBOARD, newDashboard);
        sendSuccessResponse(response, propertyTreeToString(newDashboard));
    } catch (std::exception& error) {
        SM::getLogger()->error(error.what());
        sendFailureResponse(response, error.what());
    }
}

void handleGetDashboard(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        std::string jsonString
            = propertyTreeToString(record_dashboard_t::getDashboard(id).toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleUpdateDashboard(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_dashboard_t record
            = record_dashboard_t::updateDashboard({id,
                                                   pt.get<int>("system_id"),
                                                   pt.get<std::string>("name"),
                                                   pt.get<std::string>("jsonDefinition")});
        boost::property_tree::ptree newDashboard = record.toPropertyTree();
        SM::getStreamingServer()
            ->sendMessageToAll(StreamingServer::OutgoingMessageType::UPDATE_DASHBOARD,
                               newDashboard);
        sendSuccessResponse(response, propertyTreeToString(newDashboard));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleDeleteDashboard(REQUEST_RESPONSE_PARAMS) {
    try {
        record_dashboard_t::deleteDashboard(getIntPathParam(request, 1));
        sendSuccessResponse(response, "");
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetDataPoints(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        long long before = -1;
        long long after = -1;
        std::string key = "";
        auto query_fields = request->parse_query_string();
        for (auto& field : query_fields) {
            if (field.first == KEY_QUERY_PARAM) {
                key = field.second;
            } else if (field.first == BEFORE_QUERY_PARAM) {
                before = std::stoll(field.second);
            } else if (field.first == AFTER_QUERY_PARAM) {
                after = std::stoll(field.second);
            }
        }

        const std::vector<record_data_point_t>& dps
            = record_data_point_t::getDataPoints(key, before, after);
        if (dps.empty()) {
            sendSuccessResponse(response, "{\"data_points\" : []}");
            return;
        }

        for (const record_data_point_t& dp : dps) {
            list.push_back(std::make_pair("", dp.toPropertyTree()));
        }
        pt.add_child("data_points", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

void handleGetDataFrames(REQUEST_RESPONSE_PARAMS) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ptree list;

        long long before = -1;
        long long after = -1;
        auto query_fields = request->parse_query_string();
        for (auto& field : query_fields) {
            if (field.first == BEFORE_QUERY_PARAM) {
                before = std::stoll(field.second);
            } else if (field.first == AFTER_QUERY_PARAM) {
                after = std::stoll(field.second);
            }
        }

        const std::vector<record_data_frame_t>& frames
            = record_data_frame_t::getDataFrames(before, after);
        if (frames.empty()) {
            sendSuccessResponse(response, "{\"data_frames\" : []}");
            return;
        }

        for (const record_data_frame_t& frame : frames) {
            list.push_back(std::make_pair("", frame.toPropertyTree()));
        }
        pt.add_child("data_frames", list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception& error) {
        sendFailureResponse(response, error.what());
    }
}

RestApiServer::RestApiServer() {
    // Configure server options
    server = new HttpServer();

    server->config.port = SM::getConfig()->getInt("rest_api_port", 9000);

    // Register functions

    // Status Healthcheck
    server->resource["^/v1/status$"]["GET"] = handleStatus;

    // System CRUD Functionality
    server->resource["^/v1/systems$"]["GET"] = handleGetSystems;
    server->resource["^/v1/system$"]["POST"] = handleCreateSystem;
    server->resource["^/v1/system$"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/([0-9]+)$"]["GET"] = handleGetSystem;
    server->resource["^/v1/system/([0-9]+)$"]["PUT"] = handleUpdateSystem;
    server->resource["^/v1/system/([0-9]+)$"]["DELETE"] = handleDeleteSystem;
    server->resource["^/v1/system/([0-9]+)$"]["OPTIONS"] = genericOptionsResponse;

    // Active System State
    server->resource["^/v1/system/state$"]["GET"] = handleSystemState;
    server->resource["^/v1/system/state$"]["POST"] = handleSystemStateChange;
    server->resource["^/v1/system/state$"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/active$"]["POST"] = handleSetActiveSystem;
    server->resource["^/v1/system/active$"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/active$"]["GET"] = handleGetActiveSystem;

    // Devices
    server->resource["^/v1/devices$"]["GET"] = handleGetDevices;
    server->resource["^/v1/device$"]["POST"] = handleCreateDevice;
    server->resource["^/v1/device$"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/device/([0-9]+)$"]["GET"] = handleGetDevice;
    server->resource["^/v1/device/([0-9]+)$"]["PUT"] = handleUpdateDevice;
    server->resource["^/v1/device/([0-9]+)$"]["DELETE"] = handleDeleteDevice;
    server->resource["^/v1/device/([0-9]+)$"]["OPTIONS"] = genericOptionsResponse;

    // Protocols
    server->resource["^/v1/protocols$"]["GET"] = handleGetProtocols;

    // Logs
    server->resource["^/v1/logs$"]["GET"] = handleGetLogs;
    server->resource["^/v1/logs$"]["DELETE"] = handleDeleteAllLogs;
    server->resource["^/v1/log/([0-9]+)$"]["GET"] = handleGetLog;
    server->resource["^/v1/log/([0-9]+)$"]["DELETE"] = handleDeleteLog;
    server->resource["^/v1/log/([0-9]+)$"]["OPTIONS"] = genericOptionsResponse;

    // Dashboards
    server->resource["^/v1/dashboards$"]["GET"] = handleGetDashboards;
    server->resource["^/v1/dashboard$"]["POST"] = handleCreateDashboard;
    server->resource["^/v1/dashboard$"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/dashboard/([0-9]+)$"]["GET"] = handleGetDashboard;
    server->resource["^/v1/dashboard/([0-9]+)$"]["PUT"] = handleUpdateDashboard;
    server->resource["^/v1/dashboard/([0-9]+)$"]["DELETE"] = handleDeleteDashboard;
    server->resource["^/v1/dashboard/([0-9]+)$"]["OPTIONS"] = genericOptionsResponse;

    // Data Points
    server->resource["^/v1/data_points$"]["GET"] = handleGetDataPoints;

    // Data Frames
    server->resource["^/v1/data_frames$"]["GET"] = handleGetDataFrames;

    server->on_error
        = [](const std::shared_ptr<HttpServer::Request>& request, const SimpleWeb::error_code& ec) {
              if (ec != SimpleWeb::errc::operation_canceled) {
                  SM::getLogger()->error(fmt::format("Error occurred while handling request {}: {}",
                                                     request->query_string,
                                                     ec.message()));
              }
          };

    serverThread = std::thread{&RestApiServer::_runServerThread, this};
}

RestApiServer::~RestApiServer() {
    SM::getLogger()->info("Stopping REST API Server...");
    server->stop();
    serverThread.join();
    delete server;
    SM::getLogger()->info("Stopped REST API Server.");
}

void RestApiServer::_runServerThread() {
    server->start([](unsigned short port) {
        SM::getLogger()->info(fmt::format("Started REST API server on port {}.", port));
    });
}