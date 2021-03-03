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
            mpack_start_array(&writer, numDataPoints * 6);

            for (int i = 0; i < numDataPoints; i++) {
                auto dp = in.front();
                in.pop_front();
                mpack_write_utf8_cstr(&writer, apiKey.c_str());
                mpack_write_i64(&writer, dp->timestamp);
                mpack_write_utf8_cstr(&writer, dp->key.c_str());

                if (dp->isStringType()) {
                    mpack_write_utf8_cstr(&writer, fmt::format("{}", dp->toString()).c_str());
                } else {
                    mpack_write_nil(&writer);
                }

                if (dp->isDecimalType()) {
                    switch (dp->type) {
                        case DataPointType::FLOAT32: {
                            mpack_write_float(&writer, dp->getFloat32());
                            break;
                        }
                        case DataPointType::FLOAT64: {
                            mpack_write_float(&writer, dp->getFloat64());
                            break;
                        }
                        default: {
                            mpack_write_nil(&writer);
                            break;
                        }
                    }
                } else {
                    mpack_write_nil(&writer);
                }

                if (dp->isIntegerType()) {
                    switch (dp->type) {
                        case DataPointType::UINT8: {
                            mpack_write_u8(&writer, dp->getUInt8());
                            break;
                        }
                        case DataPointType::UINT16: {
                            mpack_write_u16(&writer, dp->getUInt16());
                            break;
                        }
                        case DataPointType::UINT32: {
                            mpack_write_u32(&writer, dp->getUInt32());
                            break;
                        }
                        case DataPointType::UINT64: {
                            mpack_write_u64(&writer, dp->getUInt64());
                            break;
                        }
                        case DataPointType::INT8: {
                            mpack_write_i8(&writer, dp->getInt8());
                            break;
                        }
                        case DataPointType::INT16: {
                            mpack_write_i16(&writer, dp->getInt16());
                            break;
                        }
                        case DataPointType::INT32: {
                            mpack_write_i32(&writer, dp->getInt32());
                            break;
                        }
                        case DataPointType::INT64: {
                            mpack_write_i64(&writer, dp->getInt64());
                            break;
                        }
                        default: {
                            mpack_write_nil(&writer);
                            break;
                        }
                    }
                } else {
                    mpack_write_nil(&writer);
                }
            }
            mpack_finish_array(&writer);
            mpack_writer_destroy(&writer);

            auto out_message = std::make_shared<WsClient::OutMessage>();
            out_message->write(data, size);
            wsConnection->send(out_message, nullptr, 130);
            free(data);
            //SM::getLogger()->info(fmt::format("[{}] Wrote value chunk of {} items, {} bytes", id, numDataPoints, size));
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
