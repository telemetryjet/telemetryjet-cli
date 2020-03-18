#include "rest_api_server.h"
#include <model/records.h>
#include <services/service_manager.h>
#include <fmt/format.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <functional>
#include "utility/json_utils.h"

#define REQUEST_RESPONSE_PARAMS const std::shared_ptr<HttpServer::Response>& response, const std::shared_ptr<HttpServer::Request>& request

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

void sendSuccessResponse(const std::shared_ptr<HttpServer::Response>& response, std::string content) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Content-Type", "application/json");
    response->write(StatusCode::success_ok,content,commonHeader);
}

void sendFailureResponse(const std::shared_ptr<HttpServer::Response>& response, std::string content) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Content-Type", "application/json");
    response->write(StatusCode::client_error_bad_request,content,commonHeader);
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
    } catch (std::exception &error){
        response->write(StatusCode::client_error_bad_request, error.what());
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

        const std::vector<record_system_t> &systems = record_system_t::getSystems();
        if (systems.empty()) {
            sendSuccessResponse(response, "{\"systems\" : []}");
            return;
        }

        for (const record_system_t &system : systems) {
            list.push_back(std::make_pair("", system.toPropertyTree()));
        }
        pt.add_child("systems",list);
        sendSuccessResponse(response, propertyTreeToString(pt));
    } catch (std::exception &error){
        response->write(StatusCode::client_error_bad_request, error.what());
    }
}

void handleGetSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        record_system_t system = record_system_t::getSystem(id);
        std::string jsonString = propertyTreeToString(system.toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception &error){
        response->write(StatusCode::client_error_bad_request, error.what());
    }
}

void handleCreateSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        SM::getLogger()->info(fmt::format("Creating system from json [{}]",request->content.string()));
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_system_t record = record_system_t::createSystem(pt.get<std::string>("name"));
        std::string jsonString = propertyTreeToString(record.toPropertyTree());
        sendSuccessResponse(response, jsonString);
    } catch (std::exception &error){
        SM::getLogger()->error(error.what());
        sendFailureResponse(response, error.what());
    }
}

void handleUpdateSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        int id = getIntPathParam(request, 1);
        boost::property_tree::ptree pt = stringToPropertyTree(request->content.string());
        record_system_t::updateSystem({
            id,
            pt.get<std::string>("name")
        });
        response->write(StatusCode::success_ok);
    } catch (std::exception &error){
        response->write(StatusCode::client_error_bad_request, error.what());
    }
}

void genericOptionsResponse(REQUEST_RESPONSE_PARAMS) {
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Access-Control-Allow-Headers", "*");
    commonHeader.emplace("Content-Type", "application/json");
    response->write(StatusCode::success_ok,commonHeader);
}

void handleDeleteSystem(REQUEST_RESPONSE_PARAMS) {
    try {
        record_system_t::deleteSystem(getIntPathParam(request, 1));
        response->write(StatusCode::success_no_content);
    } catch (std::exception &error){
        response->write(StatusCode::client_error_bad_request, error.what());
    }
}

void handleGetDevices(REQUEST_RESPONSE_PARAMS) {

}

void handleCreateDevice(REQUEST_RESPONSE_PARAMS) {

}

void handleGetDevice(REQUEST_RESPONSE_PARAMS) {

}

void handleUpdateDevice(REQUEST_RESPONSE_PARAMS) {

}

void handleDeleteDevice(REQUEST_RESPONSE_PARAMS) {

}

void handleGetLogs(REQUEST_RESPONSE_PARAMS) {

}

void handleDeleteAllLogs(REQUEST_RESPONSE_PARAMS) {

}

void handleGetLog(REQUEST_RESPONSE_PARAMS) {

}

void handleDeleteLog(REQUEST_RESPONSE_PARAMS) {

}


RestApiServer::RestApiServer() {
    // Configure server options
    server = new HttpServer();

    server->config.port = SM::getConfig()->getInt("rest_api_port",9000);

    // Register functions

    // Status Healthcheck
    server->resource["^/v1/status"]["GET"] = handleStatus;

    // System CRUD Functionality
    server->resource["^/v1/systems"]["GET"] = handleGetSystems;
    server->resource["^/v1/system"]["POST"] = handleCreateSystem;
    server->resource["^/v1/system"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/([0-9]+)$"]["GET"] = handleGetSystem;
    server->resource["^/v1/system/([0-9]+)$"]["PUT"] = handleUpdateSystem;
    server->resource["^/v1/system/([0-9]+)$"]["DELETE"] = handleDeleteSystem;

    // Active System State
    server->resource["^/v1/system/state"]["GET"] = handleSystemState;
    server->resource["^/v1/system/state"]["POST"] = handleSystemStateChange;
    server->resource["^/v1/system/state"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/active"]["POST"] = handleSetActiveSystem;
    server->resource["^/v1/system/active"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/system/active"]["GET"] = handleGetActiveSystem;

    // Devices
    server->resource["^/v1/devices"]["GET"] = handleGetDevices;
    server->resource["^/v1/device"]["POST"] = handleCreateDevice;
    server->resource["^/v1/device"]["OPTIONS"] = genericOptionsResponse;
    server->resource["^/v1/device/([0-9]+)$"]["GET"] = handleGetDevice;
    server->resource["^/v1/device/([0-9]+)$"]["PUT"] = handleUpdateDevice;
    server->resource["^/v1/device/([0-9]+)$"]["DELETE"] = handleDeleteDevice;

    // Logs
    server->resource["^/v1/logs"]["GET"] = handleGetLogs;
    server->resource["^/v1/logs$"]["DELETE"] = handleDeleteAllLogs;
    server->resource["^/v1/log/([0-9]+)$"]["GET"] = handleGetLog;
    server->resource["^/v1/log/([0-9]+)$"]["DELETE"] = handleDeleteLog;

    // Dashboards

    server->on_error = [](const std::shared_ptr<HttpServer::Request>& request, const SimpleWeb::error_code & ec) {
        if (ec != SimpleWeb::errc::operation_canceled) {
            SM::getLogger()->error(fmt::format("Error occurred while handling request {}: {}",request->query_string, ec.message()));
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