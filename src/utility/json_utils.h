#ifndef TELEMETRYSERVER_JSON_UTILS_H
#define TELEMETRYSERVER_JSON_UTILS_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

static inline std::string propertyTreeToString(const boost::property_tree::ptree& in) {
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, in);
    return ss.str();
}

static inline boost::property_tree::ptree stringToPropertyTree(const std::string &in) {
    std::stringstream ss;
    ss << in;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    return pt;
}

static inline boost::property_tree::ptree streamToPropertyTree(std::stringstream &in) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(in, pt);
    return pt;
}

#endif //TELEMETRYSERVER_JSON_UTILS_H
