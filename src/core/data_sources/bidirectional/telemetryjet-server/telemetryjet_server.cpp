#include "telemetryjet_server.h"
#include "utility/encoding/base64/base64.h"

TelemetryJetServerDataSource::TelemetryJetServerDataSource(const json& definition)
        : DataSource(definition)
        , path(fmt::format("{}/api/v1/realtime/ingest", getServerPath()))
        , client(path) {
    isOnline = false;
    if (!options.contains("apiKey") || !options["apiKey"].is_string()) {
        throw std::runtime_error("TelemetryJet Server data source requires apiKey option.");
    }
    apiKey = options["apiKey"];
}

void TelemetryJetServerDataSource::open() {
    client.on_open = [this](const std::shared_ptr<WsClient::Connection>& connection) {
        wsConnection = connection;
        isOnline = true;
        SM::getLogger()->info(
                fmt::format("[{}] Opened websocket client connection to TelemetryJet server at {}.", id, path));
    };

    client.on_close = [this](const std::shared_ptr<WsClient::Connection>& connection,
                             int status,
                             const std::string& reason) {
        SM::getLogger()->info(
                fmt::format("[{}] Closed websocket client connection to TelemetryJet server at {} with status code {}.",
                            id,
                            path,
                            status));
        isOnline = false;
        clientThreadWantsExit = true;
    };

    client.on_error = [this](const std::shared_ptr<WsClient::Connection>& connection,
                             const SimpleWeb::error_code& ec) {
        SM::getLogger()->error(
                fmt::format("[{}] Error in websocket client connection to TelemetryJet server at {}. {}",
                            id,
                            path,
                            ec.message()));
        clientThreadWantsExit = true;
    };

    isOnline = false;
    startClientThread();
    reconnectTimer = std::make_unique<SimpleTimer>(5000);
    DataSource::open();
}

void TelemetryJetServerDataSource::startClientThread() {
    if (!clientThreadRunning) {
        wsConnection.reset();
        isOnline = false;
        clientThreadRunning = true;
        clientThreadWantsExit = false;
        clientThread = boost::thread([this]() { client.start(); });
    }
}

void TelemetryJetServerDataSource::stopClientThread() {
    if (clientThreadRunning) {
        isOnline = false;
        if (wsConnection != nullptr) {
            wsConnection->send_close(1000);
            wsConnection.reset();
        }
        client.stop();
        clientThread.join();
        clientThreadRunning = false;
        clientThreadWantsExit = false;
    }
}

void TelemetryJetServerDataSource::close() {
    stopClientThread();
    DataSource::close();
}

void TelemetryJetServerDataSource::update() {
    if (clientThreadWantsExit) {
        stopClientThread();
        return;
    }

    // send incoming data points to ws server
    if (isOnline) {
        while (!in.empty()) {
            char* data;
            size_t size;
            mpack_writer_t writer;
            mpack_writer_init_growable(&writer, &data, &size);

            uint64_t numDataPoints = std::min((uint64_t)in.size(), (uint64_t)10);
            mpack_start_array(&writer, numDataPoints);

            for (int i = 0; i < numDataPoints; i++) {
                auto &dp = in.front();
                in.pop_front();
                mpack_start_map(&writer, 2);
                mpack_write_cstr(&writer, "_ak");
                mpack_write_cstr(&writer, apiKey.c_str());
                mpack_write_cstr(&writer, "_ts");
                mpack_write_i64(&writer, dp->timestamp);
                mpack_write_cstr(&writer, "_k");
                mpack_write_cstr(&writer, dp->key.c_str());
                mpack_write_cstr(&writer, "_vs");
                mpack_write_cstr(&writer, dp->toString().c_str());
                mpack_write_cstr(&writer, "_vd");
                mpack_write_nil(&writer);
                mpack_write_cstr(&writer, "_vi");
                mpack_write_nil(&writer);
                mpack_finish_map(&writer);
            }
            mpack_finish_array(&writer);
            mpack_writer_destroy(&writer);

            auto out_message = std::make_shared<WsClient::OutMessage>();
            out_message->write(data, size);
            wsConnection->send(out_message, nullptr, 130);
            free(data);
            SM::getLogger()->info(fmt::format("[{}] Wrote value chunk of {} items, {} bytes", id, numDataPoints, size));
        }
        in.clear();
    }

    if (!isOnline && reconnectTimer->check()) {
        startClientThread();
    }
}

void TelemetryJetServerDataSource::checkOnline() {
    // TODO: set online member variable
}

std::string TelemetryJetServerDataSource::getServerPath() {
    if (!options.contains("hostname")) {
        throw std::runtime_error("TelemetryJet Server data source requires hostname option.");
    }

    return options["hostname"];
}
