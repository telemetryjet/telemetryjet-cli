#include "awsv4.h"

namespace AWSV4 {
    void sha256(const std::string str, unsigned char outputBuffer[SHA256_DIGEST_LENGTH]) noexcept {
        char *c_string = new char [str.length()+1];
        std::strcpy(c_string, str.c_str());
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, c_string, strlen(c_string));
        SHA256_Final(hash, &sha256);
        for (int i=0;i<SHA256_DIGEST_LENGTH;i++) {
            outputBuffer[i] = hash[i];
        }
    }

    const std::string sha256_base16(const std::string str) noexcept {
        unsigned char hashOut[SHA256_DIGEST_LENGTH];
        AWSV4::sha256(str,hashOut);
        char outputBuffer[65];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(outputBuffer + (i * 2), "%02x", hashOut[i]);
        }
        outputBuffer[64] = 0;
        return std::string{outputBuffer};
    }

    const std::string canonicalize_request(const std::string& http_request_method,
                                           const std::string& canonical_uri,
                                           const std::string& canonical_query_string,
                                           const std::string& canonical_headers,
                                           const std::string& signed_headers,
                                           const std::string& hashed_payload) noexcept {
        return http_request_method + ENDL +
               canonical_uri + ENDL +
               canonical_query_string + ENDL +
               canonical_headers + ENDL + ENDL +
               signed_headers + ENDL +
               hashed_payload;
    }

    const std::string string_to_sign(const std::string& algorithm,
                                     const std::string& timestamp,
                                     const std::string& credential_scope,
                                     const std::string& hashed_canonical_request) noexcept {
        return algorithm + ENDL +
               timestamp + ENDL +
               credential_scope + ENDL +
               hashed_canonical_request;
    }

    const std::string credential_scope(const std::string datestamp,
                                       const std::string region,
                                       const std::string service,
                                       const std::string s) noexcept {
        return datestamp + s + region + s + service + s + AWS4_REQUEST;
    }

    const std::string sign(const std::string key, const std::string msg)
    {
        unsigned char *c_key = new unsigned char[key.length() + 1];
        memcpy(c_key, (unsigned char *)key.data(), key.length());

        unsigned char *c_msg = new unsigned char[msg.length() + 1];
        memcpy(c_msg, (unsigned char *)msg.data(), msg.length());

        unsigned char * digest = HMAC(EVP_sha256(), (unsigned char*)c_key, key.length(), c_msg, msg.length(), NULL, NULL);

        delete[] c_key;
        delete[] c_msg;

        std::string signed_str = std::string((char *)digest, 32);

        return signed_str;
    }

    const std::string calculate_signature(const std::string datestamp,
                                          const std::string secret,
                                          const std::string region,
                                          const std::string service,
                                          const std::string string_to_sign) noexcept {
        std::string kDate = sign("AWS4" + secret, datestamp);
        std::string kRegion = sign(kDate, region);
        std::string kService = sign(kRegion, service);
        std::string kSigning = sign(kService, "aws4_request");
        std::string signature_str = sign(kSigning, string_to_sign);

        unsigned char *signature_data = new unsigned char[signature_str.length() + 1];
        memcpy(signature_data, (unsigned char *)signature_str.data(), signature_str.length());
        unsigned char *digest=signature_data;
        char outputBuffer[65];

        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(outputBuffer + (i * 2), "%02x", digest[i]);
        }
        outputBuffer[64] = 0;
        delete[] digest;

        return std::string(outputBuffer);
    }
}