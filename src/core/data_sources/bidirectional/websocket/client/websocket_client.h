#ifndef TELEMETRYJETCLI_WEBSOCKET_CLIENT_H
#define TELEMETRYJETCLI_WEBSOCKET_CLIENT_H

#include "client_ws.hpp"
#include "core/data_source.h"
#include "utility/timer/simple_timer.h"
#include <boost/thread.hpp>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

class WebsocketClientDataSource : public DataSource {
private:
    std::string path;
    WsClient client;
    std::shared_ptr<WsClient::Connection> wsConnection;
    boost::thread clientThread;
    std::atomic<bool> clientThreadRunning = false;
    std::atomic<bool> clientThreadWantsExit = false;
    std::unique_ptr<SimpleTimer> reconnectTimer;

    void startClientThread();
    void stopClientThread();
    std::string getServerPath();

public:
    WebsocketClientDataSource(const json& definition);
    ~WebsocketClientDataSource() = default;
    void open() override;
    void close() override;
    void update() override;
    void checkOnline() override;
};

#endif
