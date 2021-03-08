#ifndef TELEMETRYJETCLI_CSV_PARSER_H
#define TELEMETRYJETCLI_CSV_PARSER_H

#include <string>
#include <vector>

std::vector<std::string> parseCsvLine(const std::string& line, const std::string& separator = ",");

#endif
