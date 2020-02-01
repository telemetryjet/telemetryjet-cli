//
// Created by Chris Dalke on 2/1/20.
//

#include "apiServer.h"

ApiServer::ApiServer(DataCache& cache, Database &database, DeviceManager &deviceManager)
        : cache(cache), database(database), deviceManager(deviceManager) {

    server.config.port = 8080;

    server.resource["^/test$"]["GET"] = [](const std::shared_ptr<HttpServer::Response>& response, const std::shared_ptr<HttpServer::Request>& request) {
        response->write("Hello, World!\n");
    };

    server.resource["^/raw$"]["GET"] = [&cache](const std::shared_ptr<HttpServer::Response>& response, const std::shared_ptr<HttpServer::Request>& request) {
        std::string responseString;
        for (const std::string& key : cache.getKeys()){
            std::string value = std::to_string(cache.getFloat(key));
            responseString += key;
            responseString += " = ";
            responseString += value;
            responseString += "\n";
        }
        response->write(responseString);
    };

    server.on_error = [](const std::shared_ptr<HttpServer::Request>& request, const SimpleWeb::error_code & ec) {
    };

    serverThread = std::thread{&ApiServer::_runServerThread, this};
}

void ApiServer::_runServerThread() {
    server.start([](unsigned short port) {
        std::cout << "Started API Server on port " << port << "!\n";
    });
}

ApiServer::~ApiServer() {
    std::cout << "Stopping API Server...";
    server.stop();
    serverThread.join();
    std::cout << " Done!\n";
}

