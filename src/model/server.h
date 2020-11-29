#ifndef TELEMETRYJETCLI_SERVER_H
#define TELEMETRYJETCLI_SERVER_H

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

struct record_server_t {
    std::string alias;
    std::string host;
    std::string username;
    std::string password;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("alias", alias);
        pt.add("host", host);
        pt.add("username", username);
        pt.add("password", password);
        return pt;
    }

    static record_server_t createServer(const std::string& alias,
                                        const std::string& host,
                                        const std::string& username,
                                        const std::string& password);
};

#endif  // TELEMETRYJETCLI_SERVER_H
