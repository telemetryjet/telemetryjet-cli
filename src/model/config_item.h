#ifndef TELEMETRYSERVER_CONFIG_ITEM_H
#define TELEMETRYSERVER_CONFIG_ITEM_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <utility/json_utils.h>

/**
 * A Persisted Config Item.
 * Consists of a key/value pair of strings.
 *
 * All actions with this object are performed through the PersistedConfig service.
 */
struct record_config_item_t {
    std::string key;
    std::string value;

    [[nodiscard]] boost::property_tree::ptree toPropertyTree() const {
        boost::property_tree::ptree pt;
        pt.add("key", key);
        pt.add("value", value);
        return pt;
    }
};

#endif //TELEMETRYSERVER_CONFIG_ITEM_H
