#include "telemetryjet_arduino_sdk.h"
#include "utility/encoding/cobs/cobs.h"
#include "utility/messagepack/messagepack.h"

// Data types in Arduino SDK; this may differ from the Data Point types
// Because the Arduino SDK is only going to support these types
enum class ArduinoSdkDataPointType : int {
    BOOLEAN,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64
};

void TelemetryJetArduinoSdkDataSource::update() {
    SerialStreamDataSource::update();

    // Outputs: Encode and send packets
    if (isOnline && !in.empty()) {
        for (auto& dp : in) {
            uint8_t type = 0;

            // Only handle data points if the Arduino SDK supports them
            switch (dp->type) {
                case DataPointType::EVENT:
                case DataPointType::BOOLEAN: {
                    type = (uint8_t)ArduinoSdkDataPointType::BOOLEAN;
                    break;
                }
                case DataPointType::UINT8: {
                    type = (uint8_t)ArduinoSdkDataPointType::UINT8;
                    break;
                }
                case DataPointType::UINT16: {
                    type = (uint8_t)ArduinoSdkDataPointType::UINT16;
                    break;
                }
                case DataPointType::UINT32: {
                    type = (uint8_t)ArduinoSdkDataPointType::UINT32;
                    break;
                }
                case DataPointType::UINT64: {
                    type = (uint8_t)ArduinoSdkDataPointType::UINT64;
                    break;
                }
                case DataPointType::INT8: {
                    type = (uint8_t)ArduinoSdkDataPointType::INT8;
                    break;
                }
                case DataPointType::INT16: {
                    type = (uint8_t)ArduinoSdkDataPointType::INT16;
                    break;
                }
                case DataPointType::INT32: {
                    type = (uint8_t)ArduinoSdkDataPointType::INT32;
                    break;
                }
                case DataPointType::INT64: {
                    type = (uint8_t)ArduinoSdkDataPointType::INT64;
                    break;
                }
                case DataPointType::FLOAT32: {
                    type = (uint8_t)ArduinoSdkDataPointType::FLOAT32;
                    break;
                }
                case DataPointType::FLOAT64: {
                    type = (uint8_t)ArduinoSdkDataPointType::FLOAT64;
                    break;
                }
                default: {
                    continue;
                }
            }

            // Use mapping table to convert data point key to a data point ID
            // If the data point wasn't in the mapping table, ignore
            auto dimensionKeyPair = dimensions.find(dp->key);
            uint16_t key = 0;
            if (dimensionKeyPair != dimensions.end()) {
                key = dimensionKeyPair->second;
            } else {
                continue;
            }

            mpack_writer_t writer;
            size_t packetLength;
            mpack_writer_init(&writer, reinterpret_cast<char*>(tempBuffer), 32);

            // Write key and type headers
            mpack_write_u16(&writer, key);
            mpack_write_u8(&writer, type);

            switch (dp->type) {
                case DataPointType::EVENT: {
                    mpack_write_bool(&writer, true);
                }
                case DataPointType::BOOLEAN: {
                    mpack_write_bool(&writer, dp->getBoolean());
                }
                case DataPointType::UINT8: {
                    mpack_write_u8(&writer, dp->getUInt8());
                }
                case DataPointType::UINT16: {
                    mpack_write_u16(&writer, dp->getUInt16());
                }
                case DataPointType::UINT32: {
                    mpack_write_u32(&writer, dp->getUInt32());
                }
                case DataPointType::UINT64: {
                    mpack_write_u64(&writer, dp->getUInt64());
                }
                case DataPointType::INT8: {
                    mpack_write_i8(&writer, dp->getInt8());
                }
                case DataPointType::INT16: {
                    mpack_write_i16(&writer, dp->getInt16());
                }
                case DataPointType::INT32: {
                    mpack_write_i32(&writer, dp->getInt32());
                }
                case DataPointType::INT64: {
                    mpack_write_i64(&writer, dp->getInt64());
                }
                case DataPointType::FLOAT32: {
                    mpack_write_float(&writer, dp->getFloat32());
                }
                case DataPointType::FLOAT64: {
                    mpack_write_double(&writer, dp->getFloat64());
                }
                default: {
                    break;
                }
            }

            mpack_writer_destroy(&writer);
            packetLength = mpack_writer_buffer_used(&writer);

            // Use COBS (Consistent Overhead Byte Stuffing)
            // https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
            // to replace all 0x0 bytes in the packet.
            // This way, we can use 0x0 as a packet frame marker.
            packetLength = StuffData(tempBuffer, packetLength, txBuffer);

            // Compute checksum and add to front of the packet
            // We never want the checksum to == 0,
            // because that would complicate the COBS & packet frame marker logic.
            // If the checksum is going to be 0, add a single bit so that it won't be.
            uint8_t checksum = 0;
            for (size_t bufferIdx = 0; bufferIdx < packetLength; bufferIdx++) {
                checksum += (uint8_t)txBuffer[bufferIdx];
            }
            checksum = 0xFF - (checksum + 0x01);

            uint8_t checksumCorrectionByte = 0x01;
            if (checksum == 0x0) {
                // Increment byte in the front of the packet to correct the checksum
                // If the checksum was previously 0x0 (0), it will now be 0xFF (255).
                checksumCorrectionByte += 1;
                checksum = 0xFF;
            }

            // Write checksum and checksum correction byte
            serial->writeByte((uint8_t)checksum);
            serial->writeByte((uint8_t)checksumCorrectionByte);

            // Write buffer
            serial->writeBuffer(txBuffer, packetLength);
            numTxPackets++;
        }
        in.clear();
    }
}
