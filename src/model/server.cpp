#include "server.h"
#include "services/service_manager.h"

record_server_t record_server_t::createServer(const std::string& alias,
                                              const std::string& host,
                                              const std::string& username,
                                              const std::string& password)
{
    return SM::getDatabase()->createServer({alias, host, username, password});
}
