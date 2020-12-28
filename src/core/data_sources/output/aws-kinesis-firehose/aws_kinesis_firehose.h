#ifndef TELEMETRYJETCLI_AWS_KINESIS_FIREHOSE_H
#define TELEMETRYJETCLI_AWS_KINESIS_FIREHOSE_H

#include "core/data_source.h"
#include <client_https.hpp>
using HttpsClient = SimpleWeb::Client<SimpleWeb::HTTPS>;

class AwsKinesisFirehoseDataSource : public DataSource {
private:
    std::string service = "firehose";
    std::string baseUrl;
    std::string hostname;
    std::string region;
    std::string deliveryStreamName;
    std::string accessKeyId;
    std::string secretAccessKey;
    uint64_t maxRequestsPerSecond;
    std::shared_ptr<HttpsClient> client;
    std::unique_ptr<SimpleTimer> rateLimitTimer;
public:
    explicit AwsKinesisFirehoseDataSource(const json &definition);
    void open() override;
    void close() override;
    void update() override;
};

#endif