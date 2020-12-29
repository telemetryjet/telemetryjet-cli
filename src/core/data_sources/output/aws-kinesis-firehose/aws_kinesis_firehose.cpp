#include "aws_kinesis_firehose.h"

#include <nlohmann/json.hpp>
#include "utility/encoding/base64/base64.h"
#include "utility/encoding/aws-sig-v4/awsv4.h"

using json = nlohmann::json;

AwsKinesisFirehoseDataSource::AwsKinesisFirehoseDataSource(const json &definition)
    : DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires an options object", id, type));
    }
    if (!options.contains("region") || !options["region"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'region' of type String", id, type));
    }
    if (!options.contains("deliveryStreamName") || !options["deliveryStreamName"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'deliveryStreamName' of type String", id, type));
    }
    if (!options.contains("accessKeyId") || !options["accessKeyId"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'accessKeyId' of type String", id, type));
    }
    if (!options.contains("secretAccessKey") || !options["secretAccessKey"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'secretAccessKey' of type String", id, type));
    }
    region = options["region"];
    deliveryStreamName = options["deliveryStreamName"];
    accessKeyId = options["accessKeyId"];
    secretAccessKey = options["secretAccessKey"];
}

void AwsKinesisFirehoseDataSource::update() {
    rateLimitTimer->wait();
    rateLimitCount = 0;
    bool isRateLimited = false;
    while (!in.empty() && !isRateLimited) {
        SM::getLogger()->warning(fmt::format("[{}] {} records to process...", id, in.size()));
        try {
            json requestBody = {
                    {"DeliveryStreamName", deliveryStreamName},
                    {"Records", json::array()}
            };
            uint64_t numRecordsInRequest = 0;
            for (uint64_t i = 0; i < 500; i++) {
                if (in.empty()) {
                    break;
                }

                if (rateLimitCount > maxRecordsPerSecond) {
                    // Break if we are over the rate limit
                    SM::getLogger()->warning(fmt::format("[{}] Hit rate limit for Firehose, dropping data...", id));
                    isRateLimited = true;
                    break;
                }

                auto& inDataPoint = in.front();
                in.pop_front();
                requestBody["Records"].push_back({
                    {"Data", base64_encode(inDataPoint->toJson(), false)}
                });

                rateLimitCount++;
                numRecordsInRequest++;
                if (numRecordsInRequest >= 500) {
                    break;
                }
            }
            if (numRecordsInRequest == 0) {
                // Break if we did not actually send any records
                isRateLimited = true;
                return;
            }
            SM::getLogger()->warning(fmt::format("[{}] Sending {} records to Firehose...", id, requestBody["Records"].size()));

            std::string contentString = requestBody.dump();
            //SM::getLogger()->info(fmt::format("CONTENT:\n{}", contentString));
            std::string shaContentString = AWSV4::sha256_base16(contentString);
            std::pair<std::string, std::string> timestamps = getAwsTimestamps();
            std::string fullTimestamp = timestamps.first;
            std::string dateStamp = timestamps.second;

            SimpleWeb::CaseInsensitiveMultimap header;

            std::string signedHeaders = "content-type;host;x-amz-date;x-amz-target";
            std::string canonicalRequest = AWSV4::canonicalize_request(
                    "POST",
                    "/",
                    "",
                    fmt::format("content-type:application/x-amz-json-1.1\nhost:{}\nx-amz-date:{}\nx-amz-target:Firehose_20150804.PutRecordBatch", hostname, fullTimestamp),
                    signedHeaders,
                    shaContentString
                    );
            std::string canonicalRequestDigest = AWSV4::sha256_base16(canonicalRequest);
            //SM::getLogger()->info(fmt::format("CANONICAL REQUEST:\n{}", canonicalRequest));
            //SM::getLogger()->info(fmt::format("CANONICAL REQUEST DIGEST:\n{}", canonicalRequestDigest));

            std::string credentialScope = AWSV4::credential_scope(dateStamp, region, service, "/");
            //SM::getLogger()->info(fmt::format("CREDENTIAL SCOPE:\n{}", credentialScope));
            std::string stringToSign = AWSV4::string_to_sign(
                    "AWS4-HMAC-SHA256",
                    fullTimestamp,
                    credentialScope,
                    canonicalRequestDigest
                    );
            //SM::getLogger()->info(fmt::format("STRING TO SIGN:\n{}", stringToSign));

            std::string signature = AWSV4::calculate_signature(
                    dateStamp,
                    secretAccessKey,
                    region,
                    service,
                    stringToSign
                    );
            //SM::getLogger()->info(fmt::format("SIGNATURE:\n{}", signature));

            std::string authorizationHeader = fmt::format("AWS4-HMAC-SHA256 Credential={}/{}, SignedHeaders={}, Signature={}", accessKeyId, credentialScope, signedHeaders, signature);
            //SM::getLogger()->info(fmt::format("AUTHORIZATION HEADER:\n{}", authorizationHeader));

            header.emplace("Accept", "*/*");
            header.emplace("Accept-Encoding", "gzip, deflate, br");
            header.emplace("Connection", "keep-alive");
            header.emplace("Content-Type", "application/x-amz-json-1.1");
            header.emplace("Content-Length", fmt::format("{}",contentString.size()));
            header.emplace("Authorization", authorizationHeader);
            header.emplace("X-Amz-Date", fullTimestamp);
            header.emplace("X-Amz-Target", "Firehose_20150804.PutRecordBatch");

            auto response = client->request("POST", "/", contentString, header);
            //SM::getLogger()->info(fmt::format("response:\n{}", signature));
            if (response->status_code != "200 OK") {
                throw std::runtime_error(response->content.string());
            }
        } catch (const std::exception &e) {
            throw std::runtime_error(fmt::format("[{}] Failed to send data to AWS Firehose: {}", id, e.what()));
        }
    }
}

void AwsKinesisFirehoseDataSource::open() {
    hostname = fmt::format("firehose.{}.amazonaws.com", region);
    baseUrl = fmt::format("https://{}.{}.amazonaws.com/", service, region);
    client = std::make_shared<HttpsClient>(hostname, false);
    rateLimitTimer = std::make_shared<SimpleTimer>(1000);
    rateLimitCount = 0;
    DataSource::open();
    state = ACTIVE_OUTPUT_ONLY;
}

void AwsKinesisFirehoseDataSource::close() {
    client->stop();
    client.reset();
    rateLimitTimer.reset();
    DataSource::close();
}