#ifndef TELEMETRYJETCLI_KEY_VALUE_UTILS_H
#define TELEMETRYJETCLI_KEY_VALUE_UTILS_H

#include <string>
#include <list>

std::pair<std::string, std::string> parseKeyValueBuffer(std::list<uint8_t> keyValueBuffer);
std::pair<std::string, std::string> parseKeyValueString(std::string keyValueString);

#endif
