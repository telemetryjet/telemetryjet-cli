#include "serial_wrapper.h"

#include <utility>
#include "services/service_manager.h"
#include "fmt/format.h"

SerialWrapper::SerialWrapper(std::string portName, int baudRate) {
    serialPortName = std::move(portName);
    serialBaudRate = baudRate;
    serialPortOpen = false;
}

SerialWrapper::~SerialWrapper() {
    if (serialPortOpen) {
        close();
    }
}

void SerialWrapper::poll() {
    if (serialPortOpen) {
        // Add bytes to the buffer in chunks of 8
        uint8_t nextBytes[8];
        while (sp_input_waiting(serialPort) > 0) {
            int num_bytes = sp_nonblocking_read(serialPort, nextBytes, 8);
            for (int i = 0; i < num_bytes; i++) {
                serialPortBuffer.push_back(nextBytes[i]);
                serialPortNumBytes++;
            }
        }
    }
}

void SerialWrapper::pollBlocking() {
    if (serialPortOpen) {
        // Add bytes to the buffer in chunks of 8
        uint8_t nextBytes[8];
        if (sp_input_waiting(serialPort) > 0) {
            int num_bytes = sp_blocking_read_next(serialPort, nextBytes, 8, 1000);
            if (num_bytes > 0) {
                for (int i = 0; i < num_bytes; i++) {
                    serialPortBuffer.push_back(nextBytes[i]);
                    serialPortNumBytes++;
                }
            }
        }
    }
}

int SerialWrapper::getNumBytes() {
    return serialPortNumBytes;
}

std::string SerialWrapper::getPortName() {
    return serialPortName;
}

int SerialWrapper::getBaudRate() {
    return serialBaudRate;
}

bool SerialWrapper::isOpen() {
    return serialPortOpen;
}

std::list<uint8_t> &SerialWrapper::getBuffer() {
    return serialPortBuffer;
}

void SerialWrapper::clearBuffer() {
    serialPortBuffer.clear();
}

void SerialWrapper::open() {
    sp_return serialStatus = sp_get_port_by_name(serialPortName.c_str(), &serialPort);
    if (serialStatus == SP_OK) {
        serialStatus = sp_open(serialPort, SP_MODE_READ_WRITE);
        if (serialStatus == SP_OK) {
            sp_set_baudrate(serialPort, serialBaudRate);
            serialPortOpen = true;
        }
    }

    serialPortNumBytes = 0;
    if (!serialPortOpen) {
        SM::getLogger()->warning(fmt::format("Failed to initialize serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
    } else {
        SM::getLogger()->debug(fmt::format("Initialized serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
    }
}

void SerialWrapper::close() {
    sp_close(serialPort);
    serialPortOpen = false;
    SM::getLogger()->debug(fmt::format("Closed serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
}

void SerialWrapper::writeLine(std::string line) {
    int bufferLen = line.length() + 1;
    char buffer[bufferLen];
    for (int i = 0; i < line.length(); i++) {
        buffer[i] = line[i];
    }
    buffer[bufferLen - 1] = '\n';
    if (serialPortOpen) {
        sp_return writeStatus = sp_nonblocking_write(serialPort, buffer, bufferLen);
        if (writeStatus < 0) {
            SM::getLogger()->warning(fmt::format("Failed to write line to serial port [error code = {}]", writeStatus));
        }
    }
}

void SerialWrapper::writeByte(uint8_t byte) {
    uint8_t buf[1];
    buf[0] = byte;
    writeBuffer(buf, 1);
}

void SerialWrapper::writeBuffer(uint8_t* buffer, size_t length) {
    if (serialPortOpen) {
        sp_return writeStatus = sp_nonblocking_write(serialPort, buffer, length);
        if (writeStatus < 0) {
            SM::getLogger()->warning(fmt::format("Failed to write buffer to serial port [error code = {}]", writeStatus));
        }
    }
}