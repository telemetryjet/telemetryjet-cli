#ifndef H_LOGGER
#define H_LOGGER

#include <boost/log/trivial.hpp>

#define log_info(str) BOOST_LOG_TRIVIAL(info) << str

#endif
