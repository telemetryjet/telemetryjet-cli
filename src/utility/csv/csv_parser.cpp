#include "csv_parser.h"
#include<boost/tokenizer.hpp>

std::vector<std::string> parseCsvLine(const std::string& line, const std::string& separator)
{
    // https://stackoverflow.com/a/1122720
    std::vector<std::string> output;
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, boost::escaped_list_separator<char>("\\", separator, "\""));
    for (boost::tokenizer<boost::escaped_list_separator<char>>::iterator i(tok.begin()); i!=tok.end(); ++i)
    {
        output.push_back(*i);
    }
    return output;
}