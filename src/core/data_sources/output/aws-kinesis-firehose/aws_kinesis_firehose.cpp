#include "aws_kinesis_firehose.h"

#include <nlohmann/json.hpp>
#include "utility/encoding/base64/base64.h"
#include "utility/encoding/aws-sig-v4/awsv4.h"
#include <boost/thread.hpp>

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
    if (options.contains("interval") && options["interval"].is_number_integer()) {
        interval = options["interval"];
    }
    if (options.contains("maxRecordsPerSecond") && options["maxRecordsPerSecond"].is_number_integer()) {
        maxRecordsPerSecond = options["maxRecordsPerSecond"];
    }
    region = options["region"];
    deliveryStreamName = options["deliveryStreamName"];
    accessKeyId = options["accessKeyId"];
    secretAccessKey = options["secretAccessKey"];
    inputEnabled = false;
}

void AwsKinesisFirehoseDataSource::update() {
    if (rateLimitTimer->check()) {
        rateLimitCount = 0;
    }

    intervalTimer->wait();
    bool isRateLimited = false;
    try {
        while (!in.empty() && !isRateLimited) {
            boost::this_thread::interruption_point();
            if (rateLimitTimer->check()) {
                rateLimitCount = 0;
            }

            //SM::getLogger()->warning(fmt::format("[{}] {} records to process...", id, in.size()));
            json requestBody = {
                    {"DeliveryStreamName", deliveryStreamName},
                    {"Records",            json::array()}
            };
            uint64_t numRecordsInRequest = 0;
            for (uint64_t i = 0; i < 500; i++) {
                if (rateLimitTimer->check()) {
                    rateLimitCount = 0;
                }
                boost::this_thread::interruption_point();

                if (in.empty()) {
                    break;
                }

                if (maxRecordsPerSecond > 0 && rateLimitCount > maxRecordsPerSecond) {
                    // Break if we are over the rate limit
                    SM::getLogger()->warning(fmt::format("[{}] Warning: hit rate limit for Firehose, {} data points will be dropped!", id, in.size()));
                    isRateLimited = true;
                    break;
                }

                auto inDataPoint = in.front();
                in.pop_front();
                std::string recordString = base64_encode(inDataPoint->toJson(), false);
                requestBody["Records"].push_back({
                                                         {"Data", recordString}
                                                 });

                rateLimitCount++;
                numRecordsInRequest++;
                if (numRecordsInRequest >= 500) {
                    break;
                }
            }
            if (numRecordsInRequest == 0) {
                // Break if we did not actually send any records
                return;
            }
            //SM::getLogger()->warning(
            //       fmt::format("[{}] Sending {} records to Firehose...", id, requestBody["Records"].size()));

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
                    fmt::format(
                            "content-type:application/x-amz-json-1.1\nhost:{}\nx-amz-date:{}\nx-amz-target:Firehose_20150804.PutRecordBatch",
                            hostname, fullTimestamp),
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

            std::string authorizationHeader = fmt::format(
                    "AWS4-HMAC-SHA256 Credential={}/{}, SignedHeaders={}, Signature={}", accessKeyId, credentialScope,
                    signedHeaders, signature);
            //SM::getLogger()->info(fmt::format("AUTHORIZATION HEADER:\n{}", authorizationHeader));

            header.emplace("Accept", "*/*");
            header.emplace("Accept-Encoding", "gzip, deflate, br");
            header.emplace("Connection", "keep-alive");
            header.emplace("Content-Type", "application/x-amz-json-1.1");
            header.emplace("Content-Length", fmt::format("{}", contentString.size()));
            header.emplace("Authorization", authorizationHeader);
            header.emplace("X-Amz-Date", fullTimestamp);
            header.emplace("X-Amz-Target", "Firehose_20150804.PutRecordBatch");

            auto response = client->request("POST", "/", contentString, header);
            if (response->status_code != "200 OK") {
                throw std::runtime_error(response->content.string());
            }
        }
        hasError = false;
    } catch (boost::thread_interrupted& e) {
        throw e;
    } catch (std::exception &e) {
        SM::getLogger()->error(fmt::format("[{}] Failed to send data to Firehose: {}", id, e.what()));
        hasError = true;
    }
}

void AwsKinesisFirehoseDataSource::open() {
    hostname = fmt::format("firehose.{}.amazonaws.com", region);
    baseUrl = fmt::format("https://{}.{}.amazonaws.com/", service, region);
    client = std::make_shared<HttpsClient>(hostname, false);
    rateLimitTimer = std::make_shared<SimpleTimer>(1000);
    intervalTimer = std::make_shared<SimpleTimer>(interval);
    rateLimitCount = 0;
    DataSource::open();
}

void AwsKinesisFirehoseDataSource::close() {
    client->stop();
    client.reset();
    rateLimitTimer.reset();
    intervalTimer.reset();
    DataSource::close();
}