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
    FLOAT64,
    NUM_TYPES
};

void TelemetryJetArduinoSdkDataSource::update() {
    SerialStreamDataSource::update();

    if (!serial->getBuffer().empty()) {
        for (auto& inChar : serial->getBuffer()) {
            uint8_t inByte = inChar;
            if (rxIndex >= 32) {
                rxIndex = 0;
            }
            rxBuffer[rxIndex++] = inByte;

            // 0x0 pads the end of a packet
            // Reset the buffer and parse if possible
            if (inByte == 0x0) {
                if (rxIndex > 6) {
                    // If we see 0x0 and have contents in the buffer, read packet
                    // Minimum length of a packet is 5 bytes:
                    // - Checksum (1 byte)
                    // - Checksum correction byte (1 byte)
                    // - COBS header byte (1 byte)
                    // - Key (1+ byte)
                    // - Type (1+ byte)
                    // - Value (1+ byte)
                    // - Packet boundary (0x0, 1 byte)

                    auto checksumValue = (uint8_t)rxBuffer[0];
                    auto paddingByteValue = (uint8_t)rxBuffer[1];

                    // 1 - Validate checksum
                    uint8_t checksum = 0;
                    for (uint16_t bufferIdx = 0; bufferIdx < rxIndex; bufferIdx++) {
                        checksum += (uint8_t)rxBuffer[bufferIdx];
                    }
                    // Get padding/flag byte

                    if (checksum == 0xFF) {
                        // Expand COBS encoded binary string
                        // Offset the array by the two checksum bytes that are not contained in the cobs encoding
                        size_t packetLength = UnStuffData(rxBuffer + 2, rxIndex - 2, tempBuffer);

                        // Process messagepack structure
                        mpack_reader_t reader;
                        mpack_reader_init_data(&reader, reinterpret_cast<char*>(tempBuffer), packetLength);

                        uint64_t timestamp = getCurrentMillis();
                        uint16_t key = mpack_expect_u16(&reader);
                        uint8_t type = mpack_expect_u8(&reader);

                        // Convert dimension key ID into a string dimension name
                        std::string keyString;
                        for (const auto& [_keyStr, _keyInt] : dimensions) {
                            if (_keyInt == key) {
                                keyString = _keyStr;
                                break;
                            }
                        }
                        std::shared_ptr<DataPoint> dataPoint = nullptr;

                        switch ((ArduinoSdkDataPointType)type) {
                            case ArduinoSdkDataPointType::BOOLEAN: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_bool(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::UINT8: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_u8(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::UINT16: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_u16(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::UINT32: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_u32(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::UINT64: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_u64(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::INT8: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_i8(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::INT16: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_i16(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::INT32: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_i32(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::INT64: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_i64(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::FLOAT32: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_float(&reader));
                                break;
                            }
                            case ArduinoSdkDataPointType::FLOAT64: {
                                dataPoint = std::make_shared<DataPoint>(keyString, timestamp, (bool_t)mpack_expect_double(&reader));
                                break;
                            }
                        }

                        if (mpack_reader_destroy(&reader) == mpack_ok) {
                            // Write packet values as a data point
                            if (type < (uint8_t)ArduinoSdkDataPointType::NUM_TYPES && !keyString.empty() && dataPoint != nullptr) {
                                write(dataPoint);
                            }
                        } else {
                            SM::getLogger()->warning(fmt::format("[{}] Warning: Dropped packet (Failed to unpack data)", id));
                            numDroppedRxPackets++;
                        }
                    } else {
                        SM::getLogger()->warning(fmt::format("[{}] Warning: Dropped packet (Invalid checksum)", id));
                        numDroppedRxPackets++;
                    }
                } else {
                    SM::getLogger()->warning(fmt::format("[{}] Warning: Dropped packet (Packet too short)", id));
                }
                rxIndex = 0;
            }
        }
        serial->clearBuffer();
    }


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
