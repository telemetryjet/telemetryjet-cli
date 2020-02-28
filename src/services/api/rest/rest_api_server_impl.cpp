#include "rest_api_server_impl.h"
#include <services/service_manager.h>
#include <fmt/format.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

RestApiServerImpl::RestApiServerImpl() {
    // Configure server options
    server = new HttpServer();

    server->config.port = SM::getConfig()->getInt("rest_api_port",9000);
    SimpleWeb::CaseInsensitiveMultimap commonHeader;
    commonHeader.emplace("Access-Control-Allow-Origin", "*");
    commonHeader.emplace("Content-Type", "application/json");

    server->on_error = [](const std::shared_ptr<HttpServer::Request>& request, const SimpleWeb::error_code & ec) {
    };

    serverThread = std::thread{&RestApiServerImpl::_runServerThread, this};
}

RestApiServerImpl::~RestApiServerImpl() {
    SM::getLogger()->info("Stopping REST API Server...");
    server->stop();
    serverThread.join();
    delete server;
    SM::getLogger()->info("Stopped REST API Server.");
}

void RestApiServerImpl::_runServerThread() {
    server->start([](unsigned short port) {
        SM::getLogger()->info(fmt::format("Started REST API server on port {}.", port));
    });
}