#ifndef TELEMETRYSERVER_STREAMING_SERVER_H
#define TELEMETRYSERVER_STREAMING_SERVER_H

#include "server_ws.hpp"
#include <boost/property_tree/ptree.hpp>

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class StreamingServer {

private:
    WsServer* server;
    std::thread serverThread;
    void _runServerThread();

    static void handleIncomingMessage(int type,
                                      const boost::property_tree::ptree& body,
                                      const std::shared_ptr<WsServer::Connection>& connection);

public:
    // this must exactly match the outgoing streaming message type order in the frontend,
    // do not modify this enum without making the corresponding change there
    enum class IncomingMessageType : int { TEST_MESSAGE = 0 };

    // this must exactly match the incoming streaming message type order in the frontend,
    // do not modify this enum without making the corresponding change there
    enum class OutgoingMessageType : int {
        NEW_SYSTEM = 0,
        UPDATE_SYSTEM = 1,
        NEW_DEVICE = 2,
        UPDATE_DEVICE = 3,
        NEW_LOG = 4,
        NEW_DASHBOARD = 5,
        UPDATE_DASHBOARD = 6
    };

    StreamingServer();
    ~StreamingServer();

    void sendMessageToAll(OutgoingMessageType type, const boost::property_tree::ptree& body);
};

#endif  // TELEMETRYSERVER_STREAMING_SERVER_H
